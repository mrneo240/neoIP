#ifndef PATCH_H
#define PATCH_H

#include "neoIP.h"

#define NUM_PATCHES 5

struct patch_def {
	const char text[22];
	char enabled;
	uint32_t addr;
	char patch;
};

struct patch_data {
	int patch_num;
	struct patch_def patches[NUM_PATCHES];
};

#endif