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
    .global _start2

_start2:
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
	
warez_addr:
    .long   _warez_load
go_addr:
    .long   _go
old_stack_addr:
    .long   old_stack + 0x20000000
old_stack:
    .long   0x00000000