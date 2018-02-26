/* dcrp.c - scrambles and unscrambles Dreamcast-executables
 *
 * 2000 <c> Maiwe
 *
 * contact: jlo@ludd.luth.se
 *
*/

/* Notes:
 * ~~~~~~
 *
 * Dreamcast(tm) has a special method for booting regular CDs as opposed
 * to GDs. The executable file to be booted must be scrambled in a
 * certain way in order to execute properly which is what this program
 * does.
 *
 
 NeoDC 2018, ripped+adapted for ip_cracktro.
*/

void scramble(void);
void scramble2(int, char *);
void copy_data(char *);
void unscramble(void);
void unscramble2(int, char *);
void copy_data2(char *);
int  memcpy_mine(char *dest, char *src, int n);

void copy_data3(char *addr);

unsigned int permutation_key = 0;
//unsigned short permutation_buf[0x10000];
unsigned short *permutation_buf;
volatile char *input_buf;
volatile char *output_buf;
int buf_pos = 0;
int data_size =0;
int spacing = 0;

int memcpy_mine(char *cdest, char *csrc, int n)
{
	int i;
		for (i = 0; i < n; i++){
			cdest[i] = csrc[i];
		}

	return i;
}

void memset(void *dest,int len){
	int i;
	unsigned short *cdest = (unsigned short *)dest;
		for (i = 0; i < len; i++){
			cdest[i] = 0;
		}

}

char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

// Implementation of itoa()
char *itoa(int value, char* str)//, int base)
{
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;	

	// Validate base
	//if (base<2 || base>35){ *wstr='\0'; return; }
	
	// Take care of sign
	if ((sign=value) < 0) value = -value;
	
	// Conversion. Number is reversed.
	do {*wstr++ = num[value%10/*base*/];} while(value/=10/*base*/);	
	if(sign<0) *wstr++='-';
	
	*wstr='\0';

    // Reverse the string
	return wstr;//strrev(wstr);
}

//extern void bfont_draw_str_len(uint16 *buffer, char *str, int len);
int fixbin(int spc, int sel)
{
	spacing = spc;
	data_size =  303920; //File size of 1ST_READ.BIN
	input_buf =  0xac010000; //start of 1ST_READ.BIN, uncached?
	output_buf = 0xace00000; //Temp area near end of RAM
	*permutation_buf = 0xacd00000;
	memset(permutation_buf,0x10000);

	int i=0;
		
	//Not sure which we want....
	if(sel == 1){
		scramble();
		draw_string(400,130+spacing,"Done!",0xffff);
		memcpy_mine(input_buf,output_buf,data_size);
	} else if (sel == 2){
		unscramble();
		draw_string(400,130+spacing,"Done!",0xffff);
		memcpy_mine(input_buf,output_buf,data_size);
	} 

	//draw_string(20,100+spacing,"1",0xffff);
	//draw_string(20,100+spacing+40,"2",0xffff);
	
	//bfont_draw_str_len(vram_s+BUFFER_POS(62,100),(char*)output_buf+0x3FF85,5);
	//bfont_draw_str_len(vram_s+BUFFER_POS(62,140),(char*)input_buf+0x3FF85,5);
	
	//draw_string(62,100+spacing,safe_string((char*)output_buf+0x3FF85),0xffff);
	//draw_string(62,100+spacing+40,safe_string((char*)input_buf+0x3FF85),0xffff);

	return 0;
}


void scramble()
{
	//draw_string(400,100+spacing,"scramble()",0xffff);
	int j;
	char *ptr;
	int align;
	int size = data_size;

	permutation_key = size & 0x0000ffff;
	buf_pos = 0;

	align = size & 0x0000001f;
	if (align != 0)
		size -= align;

	ptr = input_buf;
	j = 0x00200000;
	do {
		while (size >= j) {
			scramble2(j, ptr);
			size = size - j;
			ptr = ptr + ((j>>2)<<2);
		}
		j = j>>1;
	} while (j >= 0x20);

	if (align != 0)
		copy_data(ptr);

}

void scramble2(int size, char *addr)
{	
	unsigned short *ptr = permutation_buf;
	int i;
	int bs = size>>5;
	unsigned short *k;
	unsigned short index;
	int m;
	unsigned short temp;

	for (i = 0; i < bs; i++)
		ptr[i] = i;

	k = &ptr[bs-1];
	for (m = bs - 1; m >= 0; m--) {
		permutation_key = (permutation_key * 2109 + 9273) & 0x7fff;
		index = (((permutation_key + 0xc000) & 0xffff) * m)>>16;

		temp = ptr[index];
		ptr[index] = *k;
		*k = temp;
		copy_data(addr + ((*k)<<5));
		k--;
	}

}

void copy_data3(char *addr)
{
	int i;
}

void copy_data(char *addr)
{
	int i;
	for (i = 0; i < 0x20 && buf_pos < data_size; i++)
		*(output_buf + (buf_pos++)) = addr[i];
}

void unscramble()
{
draw_string(400,100+spacing,"unscramble()",0xffff);
	
	int j;
	char *ptr;
	int align;
	int size = data_size;

	permutation_key = size & 0x0000ffff;
	buf_pos = 0;

	align = size & 0x0000001f;
	if (align != 0)
		size -= align;

	ptr = output_buf;
	j = 0x00200000;
	do {
		while (size >= j) {
			unscramble2(j, ptr);
			size = size - j;
			ptr = ptr + ((j>>2)<<2);
		}
		j = j>>1;
	} while (j >= 0x20);

	if (align != 0)
		copy_data2(ptr);

}

void unscramble2(int size, char *addr)
{	
	short *ptr = permutation_buf;
	int i;
	int bs = size>>5;
	unsigned short *k;
	unsigned short index;
	int m;
	unsigned short temp;

	for (i = 0; i < bs; i++)
		ptr[i] = i;

	k = &ptr[bs-1];
	for (m = bs - 1; m >= 0; m--) {
		permutation_key = (permutation_key * 2109 + 9273) & 0x7fff;
		index = (((permutation_key + 0xc000) & 0xffff) * m)>>16;

		temp = ptr[index];
		ptr[index] = *k;
		*k = temp;
		copy_data2(addr + ((*k)<<5));
		k--;
	}

}

void copy_data2(char *addr)
{
	int i;

	for (i = 0; i < 0x20 && buf_pos < data_size; i++)
		addr[i] = *(input_buf + (buf_pos++));
}
