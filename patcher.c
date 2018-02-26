/*
    This file is part of Sylverant PSO Patcher
    Copyright (C) 2011, 2012, 2013 Lawrence Sebald

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as
    published by  the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "tro.h"
#include "patches_pso.h"

#define BIN_BASE    0xac010000
#define IP_BASE     0xac008000
#define SYS_BASE    0x8c008000
#define IP_LEN      32768

extern unsigned long end;
static uint8_t *ip_bin = (uint8_t *)IP_BASE;
static uint8_t *bin = (uint8_t *)BIN_BASE;


/* GD-ROM syscall replacement. All of these extern symbols are defined in
   patch.s */
extern uint32_t gd_syscall(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t old_gd_vector;
extern uint32_t patches_count;
extern uint32_t patches[18];
//extern uint32_t server_addr;
extern uint32_t gd_syscall_len;
extern uint8_t patches_enabled;
extern void boot_stub(void *, uint32_t) __attribute__((noreturn));
extern uint32_t boot_stub_len;

extern int memcpy(char *cdest, char *csrc, int n);

uint32_t *gd_vector_addr = (uint32_t *)0xac0000bc;

void setup_patching(){
	
	patches_enabled = 0;
	const uint32_t *ptbl = patch_tables[0];

	/* Copy the GD-ROM syscall replacement... We can use from 0x8C008000 - 
	   0x8C0082FF (768 bytes) without making PSO angry.
	   Not a lot of space, but it should be enough. */
	/* First fill in all the variables that it needs... */
	old_gd_vector = *gd_vector_addr;

	/* Copy the patches over, now that we know what we're copying... */
	memcpy(&patches_count, ptbl, sizeof(uint32_t) * 23);

	/* Copy the syscall on to where it goes, and patch the address in the
	   syscalls table. */
	memcpy((char *)SYS_BASE, (char *)gd_syscall, gd_syscall_len);
	*gd_vector_addr = SYS_BASE;

	//dcache_flush_range(SYS_BASE, 768);
  

    /* The binary is in place, so let's try to boot it, shall we? */
    /*void (*f)(void) __attribute__((noreturn));
    f = (void *)((uint32)(&boot_stub) | 0xa0000000);
    f();*/
}
