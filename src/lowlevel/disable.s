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
    .global _disable_cache

_disable_cache:
    mov.l   disable_cache_k, r0
    mov.l   p2_mask, r1
    or      r1, r0
    jmp     @r0
    nop

disable_cache:
    mov.l   ccr_addr, r0
    mov.l   ccr_data_k, r1
    mov.l   @r1, r1
    mov.l   r1, @r0
    mov     #0, r3
    mov     r3, r4
    mov     r3, r5
    mov     r3, r6
    mov     r3, r7
    mov     r3, r8
    mov     r3, r9
    mov     r3, r10

    rts
    nop

    .align  4
disable_cache_k:
    .long   disable_cache
p2_mask:
    .long   0xa0000000
ccr_addr:
    .long   0xff00001c
ccr_data_k:
    .long   ccr_data
ccr_data:
    .long   0x00000808
