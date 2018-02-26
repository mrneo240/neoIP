#include "tro.h"

#define MAXCHUNK (2048*1024)

static unsigned int seed;
volatile char *input_buf;
volatile char *output_buf;
volatile int data_size = 303920; //File size of 1ST_READ.BIN
int buf_pos = 0;

//static int idx[MAXCHUNK/32/*0x10000*/];  (0x10000*4bytes = 0x40000)
volatile int *idx;

int memcpy(char *cdest, char *csrc, int n)
{
	int i;
		for (i = 0; i < n; i++){
			cdest[i] = csrc[i];
		}

	return i;
}

void memset(void *dest,int len){
	int i;
	unsigned char *cdest = (unsigned char *)dest;
		for (i = 0; i < len; i++){
			cdest[i] = 0;
		}

}

void my_srand(unsigned int n)
{
  seed = n & 0xffff;
}

unsigned int my_rand()
{
  seed = (seed * 2109 + 9273) & 0x7fff;
  return (seed + 0xc000) & 0xffff;
}

/*void load(FILE *fh, unsigned char *ptr, unsigned long sz)
{
  if(fread(ptr, 1, sz, fh) != sz)
    {
      fprintf(stderr, "Read error!\n");
      exit(1);
    }
}*/

/*void load_chunk(FILE *fh, unsigned char *ptr, unsigned long sz)
{
  static int idx[MAXCHUNK/32];
  int i;

  // Convert chunk size to number of slices 
  sz /= 32;

  // Initialize index table with unity,
     so that each slice gets loaded exactly once
  for(i = 0; i < sz; i++)
    idx[i] = i;

  for(i = sz-1; i >= 0; --i)
    {
      // Select a replacement index
      int x = (my_rand() * i) >> 16;

      // Swap 
      int tmp = idx[i];
      idx[i] = idx[x];
      idx[x] = tmp;

      // Load resulting slice 
      load(fh, ptr+32*idx[i], 32);
    }
}*/

/*void load_file(FILE *fh, unsigned char *ptr, unsigned long filesz)
{
  unsigned long chunksz;

  my_srand(filesz);

  // Descramble 2 meg blocks for as long as possible, then
  //   gradually reduce the window down to 32 bytes (1 slice) 
  for(chunksz = MAXCHUNK; chunksz >= 32; chunksz >>= 1)
    while(filesz >= chunksz)
      {
	load_chunk(fh, ptr, chunksz);
	filesz -= chunksz;
	ptr += chunksz;
      }

  // Load final incomplete slice 
  if(filesz)
    load(fh, ptr, filesz);
}*/

/*void read_file(char *filename, unsigned char **ptr, unsigned long *sz)
{
  FILE *fh = fopen(filename, "rb");
  if(fh == NULL)
    {
      fprintf(stderr, "Can't open \"%s\".\n", filename);
      exit(1);
    }
  if(fseek(fh, 0, SEEK_END)<0)
    {
      fprintf(stderr, "Seek error.\n");
      exit(1);
    }
  *sz = ftell(fh);
  *ptr = malloc(*sz);
  if( *ptr == NULL )
    {
      fprintf(stderr, "Out of memory.\n");
      exit(1);
    }
  if(fseek(fh, 0, SEEK_SET)<0)
    {
      fprintf(stderr, "Seek error.\n");
      exit(1);
    }
  load_file(fh, *ptr, *sz);
  fclose(fh);
}*/

void save(unsigned char *dst, unsigned char *src, unsigned long sz)
{
  /*if(fwrite(src, 1, sz, dst) != sz)
    {
      fprintf(stderr, "Write error!\n");
      exit(1);
    }*/
	int i;
	for (i = 0; i < sz; i++){
		*(dst + (buf_pos++)) = src[i];
	}
	
}

void save_chunk(unsigned char *dst, unsigned char *src, unsigned long sz)
{
 // static int idx[MAXCHUNK/32];
  int i;

  /* Convert chunk size to number of slices */
  sz /= 32;

  /* Initialize index table with unity,
     so that each slice gets saved exactly once */
  for(i = 0; i < sz; i++)
    idx[i] = i;

  for(i = sz-1; i >= 0; --i)
    {
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
    while(filesz >= chunksz)
      {
	save_chunk(dst, src, chunksz);
	filesz -= chunksz;
	src += chunksz;
      }

  /* Save final incomplete slice */
  if(filesz)
    save(dst, src, filesz);
}

/*void write_file(char *filename, unsigned char *ptr, unsigned long sz)
{
  /*FILE *fh = fopen(filename, "wb");
  if(fh == NULL)
    {
      fprintf(stderr, "Can't open \"%s\".\n", filename);
      exit(1);
    }
  save_file(fh, ptr, sz);
  fclose(fh);
}*/

void descramble()
{
/*  unsigned char *ptr = NULL;
  unsigned long sz = 0;
  FILE *fh;

  read_file(src, &ptr, &sz);

  fh = fopen(dst, "wb");
  if(fh == NULL)
    {
      fprintf(stderr, "Can't open \"%s\".\n", dst);
      exit(1);
    }
  if( fwrite(ptr, 1, sz, fh) != sz )
    {
      fprintf(stderr, "Write error.\n");
      exit(1);
    }
  fclose(fh);
  free(ptr);*/
}

void scramble()
{
  save_file(output_buf, input_buf, data_size);
}

void fixbin(int choice)
{  
	data_size	= 1468208; //File size of 1ST_READ.BIN
	input_buf 	= 0xac010000; //start of 1ST_READ.BIN, uncached?
	output_buf	= 0xace00000; //Temp area near end of RAM
	idx 		= 0xACDB0000; //idx buffer pointer size 0x40000
	
	memset(idx,0x40000);
	memset(0xACE00000,data_size);
	
//  if(choice-1)
//    scramble();
//  else
//    descramble();
	save_file(output_buf, input_buf, data_size);
	
	memset(0x8c010000,data_size);
	sleep_ms(100);
	memcpy(input_buf-0x20000000,output_buf,data_size);
	sleep_ms(100);
}
