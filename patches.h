#include "tro.h"

struct patch_def {
	char text[20];
	uint32_t addr;
	char patch;
};

struct patch_data {
	int patch_num;
	struct patch_def patches[4];
};

static struct patch_data PATCHES = {.patch_num = 4,
				.patches = {
				{"ZZZZZZZZZZZZZZZZZZZ",0x8c010000,'00'},
				{"ZZZZZZZZZZZZZZZZZZZ",0x8c010000,'00'},
				{"ZZZZZZZZZZZZZZZZZZZ",0x8c010000,'00'},
				{"ZZZZZZZZZZZZZZZZZZZ",0x8c010000,'00'}
				}
};