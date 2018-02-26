#include "tro.h"
#include "maple.h"

#define MAPLE(x) (*(volatile unsigned long *)(0xa05f6c00+(x)))

static struct mapledev dev[4];

static struct {
  unsigned int rbuf[4][256];
  unsigned int sbuf[4*(2+2)];
  char padding[32];
} *dmabuffer, dmabuffer0;

static int pending, lock;

static unsigned char xdmabuffer[ 1024 + 1024 + 4 + 4 + 32 ];

void maple_init()
{
  int i;

  MAPLE(0x8c) = 0x6155404f;
  MAPLE(0x10) = 0;
  MAPLE(0x80) = (50000<<16)|0;
  MAPLE(0x14) = 1;
  dmabuffer =
    (void*)(((((unsigned long)(void *)&dmabuffer0)+31)&~31)|0xa0000000);
  for(i=0; i<4; i++)
    dev[i].func = dev[i].xfunc = 0;
  pending = lock = 0;
}

void maple_wait_dma()
{
  while(MAPLE(0x18) & 1)
    ;
}

void *maple_docmd(int port, int unit, int cmd, int datalen, void *data)
{
  unsigned long *sendbuf, *recvbuf;
  int to, from;

  if(lock)
    return 0;
  lock = 1;

  port &= 3;

  /* Compute sender and recipient address */
  from = port << 6;
  to = (port << 6) | (unit>0? ((1<<(unit-1))&0x1f) : 0x20);

  /* Max data length = 255 longs = 1020 bytes */
  if(datalen > 255)
    datalen = 255;
  else if(datalen < 0)
    datalen = 0;

  /* Allocate a 1024 byte receieve buffer at the beginning of
     dmabuffer, with proper alignment.  Also mark the buffer as
     uncacheable.                                               */
  recvbuf =
    (unsigned long *) (((((unsigned long)xdmabuffer)+31) & ~31) | 0xa0000000);

  /* Place the send buffer right after the receive buffer.  This
     automatically gives proper alignment and uncacheability.    */
  sendbuf =
    (unsigned long *) (((unsigned char *)recvbuf) + 1024);

  /* Make sure no DMA operation is currently in progress */
  maple_wait_dma();

  /* Set hardware DMA pointer to beginning of send buffer */
  MAPLE(0x04) = ((unsigned long)sendbuf) & 0xfffffff;

  /* Setup DMA data.  Each message consists of two control words followed
     by the request frame.  The first control word determines the port,
     the length of the request transmission, and a flag bit marking the
     last message in the burst.  The second control word specifies the
     address where the response frame will be stored.  If no response is
     received within the timeout period, -1 will be written to this address. */

  /* Here we know only one frame should be send and received, so
     the final message control bit will always be set...          */
  *sendbuf++ = datalen | (port << 16) | 0x80000000;

  /* Write address to receive buffer where the response frame should be put */
  *sendbuf++ = ((unsigned long)recvbuf) & 0xfffffff;

  /* Create the frame header.  The fields are assembled "backwards"
     because of the Maple Bus big-endianness.                       */
  *sendbuf++ = (cmd & 0xff) | (to << 8) | (from << 16) | (datalen << 24);

  /* Copy parameter data, if any */
  if(datalen > 0) {
    unsigned long *param = data;
    int i;
    for(i=0; i<datalen; i++)
      *sendbuf++ = *param++;
  }

  /* Frame is finished, and DMA list is terminated with the flag bit.
     Time to activate the DMA channel.                                */
  MAPLE(0x18) = 1;

  /* Wait for the complete cycle to finish, so that the response
     buffer is valid.                                            */
  maple_wait_dma();

  lock = 0;

  /* Return a pointer to the response frame */
  return recvbuf;
}

struct mapledev *check_pads()
{
  unsigned int *buf = dmabuffer->sbuf;
  int i;

  if(lock)
    return dev;
  lock = 1;

  if(MAPLE(0x18) & 1) {
    lock = 0;
    return dev;
  }

  if(pending) {
    pending = 0;

    for(i=0; i<4; i++) {
      unsigned char *r = (unsigned char *)dmabuffer->rbuf[i];
      if(*r == MAPLE_RESPONSE_DATATRF && r[3]>=3 && (r[7] & dev[i].xfunc)) {
	dev[i].cond.data[0] = ((unsigned int *)r)[2];
	dev[i].cond.data[1] = ((unsigned int *)r)[3];
	dev[i].func = dev[i].xfunc;
      } else if(*r == MAPLE_RESPONSE_DEVINFO && r[3]>=28 &&
		(r[7] & (MAPLE_FUNC_CONTROLLER | MAPLE_FUNC_KEYBOARD))) {
	dev[i].xfunc = ((r[7] & MAPLE_FUNC_CONTROLLER)?
			MAPLE_FUNC_CONTROLLER : MAPLE_FUNC_KEYBOARD);
	dev[i].func = 0;
      } else
	dev[i].func = dev[i].xfunc = 0;
    }
  }

  for(i=0; i<4; i++) {
    unsigned int addr = i<<(6+8);
    addr |= (0x20|addr)<<8;
    *buf++ = (i << 16) | (i==3? 0x80000000 : 0);
    *buf++ = ((unsigned long)(void*)dmabuffer->rbuf[i]) & 0xfffffff;
    switch(dev[i].xfunc) {
     case MAPLE_FUNC_CONTROLLER:
     case MAPLE_FUNC_KEYBOARD:
       buf[-2] |= 1;
       *buf++ = MAPLE_COMMAND_GETCOND | addr | (1<<24);
       *buf++ = dev[i].xfunc<<24;
       break;
     default:
       *buf++ = MAPLE_COMMAND_DEVINFO | addr;
       break;
    }
  }
  MAPLE(0x04) = ((unsigned long)(void*)dmabuffer->sbuf) & 0xfffffff;
  MAPLE(0x18) = 1;

  pending = 1;
  lock = 0;
  return dev;
}
