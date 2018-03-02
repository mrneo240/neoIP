#ifndef PATCH_H
#define PATCH_H

#include "neoIP.h"

struct patch_def {
	const char text[22];
	char enabled;
	char type; //1=char, 2=short, 4=int
	uint32_t addr;
	uint32_t patch;
};

struct patch_data {
	int patch_num;
	struct patch_def patches[NUM_PATCHES];
};

#endif