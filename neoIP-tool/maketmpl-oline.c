/*  maketmpl.c

DESCRIPTION

    Combines three files (template, bootstrap 1, bootstrap 2) into a single
    new IP.BIN template.

*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char **argv)
{
    FILE *ip, *boot1, *boot2, *ip_out;
    char ip_buf[0x8000];

    if (argc != 4)
    {
        fprintf (stderr, "Usage: %s (IP.TMPL) (BOOT1.BIN) (HACKED IP)\\n", argv[0]);
        exit (1);
    }

    memset (ip_buf, 0, sizeof (ip_buf));    

    ip = fopen (argv[1], "rb");
    boot1 = fopen (argv[2], "rb");
    //boot2 = fopen (argv[3], "r");
	
	fseek(boot1,0L,SEEK_END);
	int boot1_length=ftell(boot1);
	fseek(boot1,0L,SEEK_SET);
	
	ip_out= fopen(argv[3], "wb");

    if (!ip)
        fprintf (stderr, "Unable to open '%s'.\\n", argv[1]);
    else if (!boot1)
        fprintf (stderr, "Unable to open '%s'.\\n", argv[2]);
    /*else if (!boot2)
        fprintf (stderr, "Unable to open '%s'.\n", argv[3]);*/
    else if (!fread (ip_buf, sizeof (ip_buf), 1, ip))
        fprintf (stderr, "Unable to read IP template.\\n");
    else if (!fread (ip_buf + 0x3800, 1, boot1_length, boot1))
        fprintf (stderr, "Unable to read bootstrap 1.\\n");
    //else if (!fread (ip_buf + 0x6000, 1, 0x2000, boot2))
    //    fprintf (stderr, "Unable to read bootstrap 2.\n");
    else{
		//patch region free
		ip_buf[0x30] = 'J';
		ip_buf[0x31] = 'U';
		ip_buf[0x32] = 'E';
		
		//patch VGA
		ip_buf[0x3D] = '1';
		
		//region free part2
		unsigned char rawData[75] =
		{
			0x46, 0x6F, 0x72, 0x20, 0x4A, 0x41, 0x50, 0x41, 0x4E, 0x2C, 0x54, 0x41, 0x49, 0x57, 0x41, 0x4E, 
			0x2C, 0x50, 0x48, 0x49, 0x4C, 0x49, 0x50, 0x49, 0x4E, 0x45, 0x53, 0x2E, 0x0E, 0xA0, 0x09, 0x00, 
			0x46, 0x6F, 0x72, 0x20, 0x55, 0x53, 0x41, 0x20, 0x61, 0x6E, 0x64, 0x20, 0x43, 0x41, 0x4E, 0x41, 
			0x44, 0x41, 0x2E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0E, 0xA0, 0x09, 0x00, 
			0x46, 0x6F, 0x72, 0x20, 0x45, 0x55, 0x52, 0x4F, 0x50, 0x45, 0x2E, 
		} ;
		
		int i;
		for(i=0;i<75;i++){
			ip_buf[0x3704+i] = rawData[i];
		}
		
		//int data_size_OFFSET = 0x8c00d454;
		//((int*)ip_buf)[((data_size_OFFSET-0x8c00b800)+0x3800)/4] = 1541108;

        fwrite (ip_buf, sizeof (ip_buf), 1, ip_out);
	}

    fprintf (stderr, "Done!\\n");
	return 0;
}
