#include "neoIP.h"

int memcmp(const void *src1, const void *src2, uint32_t len)
{
	const unsigned char *c1 = src1, *c2 = src2;
	int d = 0;

	while (len--) {
		d = (int)*c1++ - (int)*c2++;
		if (d)
			break;
	}

	return d;
}

int memcpy(const void *dest, const void *src, uint32_t len)
{
	int i;
	unsigned char *cdest = dest, *csrc = src;
	for (i = 0; i < len; i++) {
		cdest[i] = csrc[i];
	}
	return i;
}

void memset(const void *dest, uint32_t len)
{
	int i;
	unsigned char *cdest = (unsigned char *)dest;
	for (i = 0; i < len; i++) {
		cdest[i] = 0;
	}
}

void strcpy(char* dest, char* src)
{
	int i=0;
	while(src[i] != '\0' ) {
		dest[i]=src[i++];
	}
	dest[i]=0;
}