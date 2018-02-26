#include "tro.h"

struct patch_def {
	const char text[22];
	char enabled;
	uint32_t addr;
	char patch;
};

struct patch_data {
	int patch_num;
	struct patch_def patches[4];
};

static struct patch_data PATCHES = {.patch_num = 4,
				.patches = {
				{"ZZZZZZZZZZZZZZZZZZZZZ",0,0x8c010000,'00'},
				{"AZZZZZZZZZZZZZZZZZZZZ",0,0x8c010000,'00'},
				{"XZZZZZZZZZZZZZZZZZZZZ",0,0x8c010000,'00'},
				{"CZZZZZZZZZZZZZZZZZZZZ",0,0x8c010000,'00'}
				}
};