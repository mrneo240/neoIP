#ifndef __LIBC_H
#define __LIBC_H

int memcmp(const void *s1, const void *s2, uint32_t len);
int memcpy(const void *cdest, const void *csrc, uint32_t len);
void memset(const void *dest, uint32_t len);
void strcpy(char *dest, char *src);

#endif