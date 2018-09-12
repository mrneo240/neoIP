#include <kos.h>
#include <math.h>
#include <stdio.h>
KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);

typedef struct {int rows; int cols; unsigned char* data;} sImage;
sImage	originalImage;

typedef struct
        {
                unsigned char RGB[3];
        }RGB;

uint16 *backbuffer_s;
uint32 *backbuffer_l;

#define PACK_PIXEL(r, g, b) ( ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3) )
#define DRAW_PIXEL(x, y, c,k) vram_s[x + yPos[k]] = c;

int c = 0;
unsigned int x=0;
int i=0;
int chars = 0;
int r=0;
int p=0;
int rr=0;
int q=0;
long fileSize;
int nColors;
int vectorSize;
float u;
float v;
unsigned int u1,v1,o;
float xs;
float ys;
float count;
int index2;
float a;
float r1;
float w;
unsigned int y;
int k = 0;
int xi;
int yi;
int j;
int src_index;
int mXres=256;
int mYres=256;
unsigned int mLUT[262144*2];
int yPos[480];
float pi = 3.1415926;

RGB	pChar;
unsigned char	someChar;
int balance[1024][1024];

long getImageInfo(FILE* inputFile, long offset, int numberOfChars)
{
  unsigned char		*ptrC;
  long			value = 0L;
  unsigned char		dummy;
  int			i;

  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, offset, SEEK_SET);

  for(i=1; i<=numberOfChars; i++)
  {
    fread(ptrC, sizeof(char), 1, inputFile);
    /* calculate value based on adding bytes */
    value = (long)(value + (*ptrC)*(pow(256, (i-1))));
  }
  return(value);

} /* end of getImageInfo */

// Flip page
uint16* backBuffer=NULL;
uint16* frontBuffer=NULL;

void clearScreen(uint16 c) {
   int x,y;

   for(x=0;x<640;x++) {
      for(y=0;y<480;y++)
         *(backBuffer+(x+(640*y)))=c;
   }
}

void initBuffers(void) {
   frontBuffer=vram_s;
   backBuffer=(frontBuffer+(640*480));  //dumb hack ill fix later

   vid_empty();

   vid_set_start(backBuffer);

   vid_empty();

   vid_set_start(frontBuffer);
}

void swapBuffers(void) {
   uint16* temp;
   
   temp=backBuffer;
   backBuffer=frontBuffer;
   frontBuffer=temp;
   
   vid_waitvbl();

   vid_set_start(frontBuffer);
}

extern uint8 romdisk[];


KOS_INIT_ROMDISK(romdisk);

int main(int argc, char **argv) {
vid_set_mode(DM_320x240, PM_RGB565);
initBuffers();

 for( j=0; j < 240; j++ ) 
   
    {
      	   
			yPos[j] = j*320;       
}

    
    for( j=0; j < mYres*2; j++ ) 
   	 for( i=0; i < mXres*2; i++ )
    {
      	    xs = i-32;
			ys = j;
			
			a = atan2(ys,xs);
			r1 = sqrt( xs * xs + ys * ys );

			v = 20 / r1;
			u = 0.25 * a / pi;
			u = u * 512;
			v = v * 512;

	        mLUT[k++] = (int) u &511;
	 		mLUT[k++] = (int) v &511;       
}



FILE* img = fopen("/rd/5580.bmp", "rb");
if (!img)
    {
      for (r=0;r<640;r++){

	 int c = PACK_PIXEL (255,255,255);
    	printf ("%d",r);
      DRAW_PIXEL (r,64,c,60);
}
    }
 originalImage.cols = (int)getImageInfo(img, 18, 4);
 originalImage.rows = (int)getImageInfo(img, 22, 4);
 fileSize = getImageInfo(img, 2, 4);
 nColors = getImageInfo(img, 46, 4);
 vectorSize = fileSize - (14 + 40 + 4*nColors);

// go to start of image data
  fseek(img, (54 + 4*nColors), SEEK_SET);
	RGB tmp;
// read bmp to array
  for(r=0; r<=512 - 1; r++)
  {
    for(p=0; p<=512 - 1; p++)
    {
      fread(&tmp, sizeof(RGB),1, img);
      balance[p][r] = PACK_PIXEL(tmp.RGB[0],tmp.RGB[1],tmp.RGB[2]); 		     
    }
  }

  while(1) {
  	count +=1.8;
  	k=0;
  for (yi = 240;yi--; ) {
	
		for (xi = 320; xi--;  ) {
		
			o = (yi << 9)+xi;
			u1 = mLUT[(o << 1)+0] + count;
			v1 = mLUT[(o << 1)+1]+  count;	
			
			int color = balance[u1&511][v1&511];
      		DRAW_PIXEL (xi,yi,color,yi);		     
							
					}
				}			
      
   //   swapBuffers();

}
   return 0;
}





