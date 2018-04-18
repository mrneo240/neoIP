! ipwarez, a commercial selfboot patched Dreamcast bootsector
!
! Copyright (C) 2001 Jacob Alberty <calberty@home.com>
! Copyright (C) 2002 Scott Robinson <scott@auburnvo.org>
!
! This program is free software; you can redistribute it and/or modify
! it under the terms of the GNU Lesser General Public License as published by
! the Free Software Foundation; either version 2.1 of the License, or
! (at your option) any later version.
!
! This program is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
! GNU General Public License for more details.
!
! You should have received a copy of the GNU Lesser General Public License
! along with this program; if not, write to the Free Software
! Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    .text
    .global _go

_go:
    mov.l   entry_addr_k,r0
    mov.l   @r0,r0
    lds     r0,pr
    mov.l   sr_data_k,r0
    mov.l   @r0,r0  
    ldc     r0,sr   
    mov     #0,r1
    mov     r1,r2
    mov     r1,r3
    mov     r1,r11
    mov     r1,r12
    mov     r1,r13
    mov.l   vbr_data_k,r15
    mov.l   @r15,r15
    ldc     r15,vbr
    mov.l   fpscr_data_k,r0
    mov.l   @r0,r0    
    lds     r0,fpscr
    mov.l   base_addr_k,r0
    mov.l   @r0,r0
    jsr     @r0
    mov     r0,r4

    .align  4
entry_addr_k:
    .long   entry_addr
entry_addr:
    .long   0x8cc00000
base_addr_k:
    .long   base_addr
base_addr:
    .long   0x8c010000
sr_data_k:
    .long   sr_data
sr_data:
    .long   0x600000f0
vbr_data_k:
    .long   vbr_data
vbr_data:
    .long   0x8c00f400
fpscr_data_k:
    .long   fpscr_data
fpscr_data:
    .long   0x40001
