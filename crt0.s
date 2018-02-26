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
    .global start2

start2:
    ! First, make sure to run in the P2 area.
    mov.l   setup_cache_addr, r0
    mov.l   p2_mask, r1
    or      r1, r0
    jmp     @r0
    nop

setup_cache:
    ! Now that we are in P2, it's safe to enable the cache.
    mov.l   ccr_addr, r0
    mov.w   ccr_data, r1
    mov.l   r1, @r0

    ! After changing CCR, eight instructions must be executed before it's
    ! safe to enter a cached area such as P1. This will easily be taken care
    ! of with setting up the stack and some nops.

    sts.l   pr, @-r15           ! 1
    mov.l   old_stack_addr, r0  ! 2
    mov.l   r15, @r0            ! 3
    mov.l   new_stack, r15      ! 4
    mov.l   disable_addr, r0    ! 5
    mov     #0, r14             ! 6
    nop                         ! 7
    nop                         ! 8
    jsr     @r0                 ! go (disable the cache!)
    nop

    mov.l   warez_addr, r0      ! enable the selfboot patches
    jsr     @r0
    nop

    mov.l   go_addr, r2
    jsr     @r2                 ! _go (actually do stuff!)
    nop

    ! Put back the old stack pointer and PR.
    mov.l   old_stack, r15

    lds.l   @r15+, pr
    rts
    nop

    .align  4
p2_mask:
    .long   0xa0000000
setup_cache_addr:
    .long   setup_cache
disable_addr:
    .long   _disable_cache
warez_addr:
    .long   _warez_load
go_addr:
    .long   _go
ccr_addr:
    .long   0xff00001c
ccr_data:
    .word   0x090b
    .word   0x0000
old_stack_addr:
    .long   old_stack + 0x20000000
old_stack:
    .long   0x00000000
new_stack:
    .long   0x8d000000
