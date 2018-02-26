/*  maketmpl.c

DESCRIPTION

    Combines three files (template, bootstrap 1, bootstrap 2) into a single
    new IP.BIN template.

*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "boot1.h"

union
{
    unsigned int integer;
    unsigned char byte[4];
} tempInt;

int main (int argc, char **argv)
{
    FILE *ip, *binary, *ip_out;
    char ip_buf[0x8000];
    int file_size;

    if (argc != 4)
    {
        fprintf (stderr, "Usage: %s (IP.BIN Original) (1ST_READ.BIN) (Hacked IP.BIN)\n", argv[0]);
        exit (1);
    }

    memset (ip_buf, 0, sizeof (ip_buf));    

    ip = fopen (argv[1], "rb");
    binary = fopen(argv[2], "r");
	ip_out= fopen(argv[3], "wb");
	
   if(binary==NULL) {
      fprintf(stderr,"%s not found!\n",argv[2]);
	  exit(1);
   } else {
      fseek(binary,0L,SEEK_END);
      tempInt.integer=ftell(binary);
      fclose(binary);
   }
    
    if (!ip)
        fprintf (stderr, "Unable to open '%s'.\n", argv[1]);
    else if (!fread (ip_buf, sizeof (ip_buf), 1, ip))
        fprintf (stderr, "Unable to read Original IP.BIN\n");
    else { 
		memcpy(ip_buf+0x3800,boot1,sizeof(boot1));
		//offset of data_size in boot1.bin+0x3800
		ip_buf[0x4CEC]=tempInt.byte[0];
		ip_buf[0x4CED]=tempInt.byte[1];
		ip_buf[0x4CEE]=tempInt.byte[2];
		ip_buf[0x4CEF]=tempInt.byte[3];
		fwrite (ip_buf, sizeof (ip_buf),1,ip_out);
		fclose(ip_out);
	}

    fprintf (stderr, "Done!\n");
	return 0;
}
