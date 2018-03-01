#include "neoIP.h"

#define MAXCHUNK (2048*1024)

static unsigned int seed;
volatile char *input_buf;
volatile char *output_buf;
volatile int data_size = 3128; //File size of 1ST_READ.BIN
int buf_pos = 0;

volatile int *idx; //idx[MAXCHUNK/32/*0x10000*/];  (0x10000*4bytes = 0x40000)

void my_srand(unsigned int n)
{
	seed = n & 0xffff;
}

unsigned int my_rand()
{
	seed = (seed * 2109 + 9273) & 0x7fff;
	return (seed + 0xc000) & 0xffff;
}

void save(unsigned char *dst, unsigned char *src, uint32_t sz)
{
	int i;
	for (i = 0; i < sz; i++) {
		*(dst + (buf_pos++)) = src[i];
	}

}

void save_chunk(unsigned char *dst, unsigned char *src, uint32_t sz)
{
// static int idx[MAXCHUNK/32];
	int i;

	/* Convert chunk size to number of slices */
	sz /= 32;

	/* Initialize index table with unity,
	   so that each slice gets saved exactly once */
	for(i = 0; i < sz; i++)
		idx[i] = i;

	for(i = sz-1; i >= 0; --i) {
		/* Select a replacement index */
		int x = (my_rand() * i) >> 16;

		/* Swap */
		int tmp = idx[i];
		idx[i] = idx[x];
		idx[x] = tmp;

		/* Save resulting slice */
		save(dst, src+32*idx[i], 32);
	}
}

void save_file(unsigned char *dst, unsigned char *src, uint32_t filesz)
{
	uint32_t chunksz;

	my_srand(filesz);

	/* Descramble 2 meg blocks for as long as possible, then
	   gradually reduce the window down to 32 bytes (1 slice) */
	for(chunksz = MAXCHUNK; chunksz >= 32; chunksz >>= 1)
		while(filesz >= chunksz) {
			save_chunk(dst, src, chunksz);
			filesz -= chunksz;
			src += chunksz;
		}

	/* Save final incomplete slice */
	if(filesz)
		save(dst, src, filesz);
}

void fixbin(int choice)
{
	//data_size =  303920; //File size of 1ST_READ.BIN
	input_buf 	= 0xAC010000; //start of 1ST_READ.BIN
	output_buf	= 0xACE00000; //Temp area near end of RAM
	idx 		= 0xACDB0000; //idx buffer pointer size 0x40000
	choice = ((data_size+32-1)&-32);

	//memset(idx,0x40000);
	sq_clr(idx, 0x40000);
	//memset(0xACE00000,data_size);
	sq_clr(0xACE00000, choice);

	save_file(output_buf, input_buf, data_size);

	//memset(input_buf,data_size);
	sq_clr(input_buf, choice);
	memcpy(input_buf-0x20000000,output_buf,data_size);
}
