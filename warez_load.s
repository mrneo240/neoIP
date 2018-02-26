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
    .global _warez_load

_warez_load:
    sts.l   pr, @-r15

    bsr     ResetGD
    nop

    bsr     Init_drive
    nop

    mova    p1search, r0
    mov     r0, r4
    mova    p1replace, r0
    mov     r0, r5
    bsr     patch
    nop

    mova    p2search, r0
    mov     r0, r4
    mova    p2replace, r0
    mov     r0, r5
    bsr     patch
    nop

    mov     #0x18, r4
    mov     #0, r5
    bsr     Exec_Cmd
    nop

    lds.l   @r15+, pr
    rts
    nop

    .align  4
p1search:
    .byte   0x00, 0x0e, 0xa0, 0x00, 0xa4, 0x00, 0x00, 0x24, 0x00, 0x08, 0, 0
p1replace:
    .byte   0x00, 0x0e, 0xa0, 0x00, 0xa4, 0x00, 0x00, 0x28, 0x00, 0x08, 0, 0

p2search:
    .byte   0x7c, 0x67, 0x20, 0x35, 0x01, 0x8d, 0x0f, 0xe3, 0x73, 0x65, 0, 0
p2replace:
    .byte   0x73, 0x60, 0x0f, 0xc9, 0x80, 0xcb, 0x0f, 0xe3, 0x03, 0x65, 0, 0

ResetGD:
    sts.l   pr, @-r15

    mova    GDreset, r0
    mov.l   @r0, r4
    mova    GDresetval, r0
    mov.l   @r0, r5

    mov.l    r5, @r4

    mova    Aflushstart, r0
    mov.l   @r0, r4
    mova    Aflushend, r0
    mov.l   @r0, r5

aflushlp:
    mov.l   @r4, r0
    add     #4, r4

    cmp/eq  r4, r5
    bf      aflushlp

    bsr     GdInitSystem
    nop

    lds.l   @r15+, pr
    rts
    nop

    .align  4

GDreset:
    .long   0xa05f74e4
GDresetval:
    .long   0x1fffff
Aflushstart:
    .long   0xa0000000
Aflushend:
    .long   0xa0200000

!r4, r5, r6 args.
    .align  4
Exec_Cmd:
    sts.l   pr, @-r15

    bsr     GdReqCmd
    nop
    
    mov     r0, r4
    mova    cmdnum, r0
    mov.l   r4, @r0

    mova    cmdnum, r0
    mov.l   @r0, r4

Execlp1:
    bsr     GdExecServer
    nop

    mova    cmdnum, r0
    mov.l   @r0, r4
    mova    stat, r0
    mov     r0, r5

    bsr     GdGetCmdStat
    nop

    cmp/eq  #1, r0            ! busy? -> loop
    bt      Execlp1

    cmp/eq  #2, r0
    bt      ExecSuccess

    mova    stat, r0
    mov.l   @r0, r0

    lds.l   @r15+, pr
    rts
    nop

ExecSuccess:
    mov     #0, r0

    lds.l   @r15+, pr
    rts
    nop

    .align  4
cmdnum:
    .long   0
stat:
    .long   0, 0, 0, 0

!------------------------------------------------------------------------------
Init_drive:
    sts.l   pr, @-r15

    mov     #0, r4
    mov.l   r4, @-r15
    
inilp:    
    mov     #24, r4     ! Init disc
    mov     #0, r5

    bsr     Exec_Cmd
    nop

    cmp/eq  #0, r0
    bt      No_Error

    mov     r0, r1
    mov.l   @r15+, r0
    add     #1, r0

    cmp/eq  #8, r0
    bt      Init_Error2
    
    mov.l   r0, @-r15
    bra     inilp
    nop

Init_Error2:
    mov     r1, r0
    bra     Init_Error
    nop

No_Error:
    mov.l    @r15+, r0

    mova    iniparam, r0
    mov     r0, r4
    
    bsr     GdGetDrvStat
    nop

    mova    iniparam, r0    ! Get disc type
    add     #4, r0
    mov.l   @r0, r4    
    nop
    mov     r4, r0
    nop
    
    cmp/eq  #32, r0         ! 0x10 for mode 1 CD, 0x20 for mode 2 CD, 0x80 for GD.
    bt      id_mode2

id_mode1:
    mova    id_m1val, r0
    mov     r0, r5
    mova    id_par2, r0
    mov     r0, r4
    mov.l   @r5, r6
    mov.l   r6, @r4
    bra     id_cont1
    nop

id_mode2:
    mova    id_m2val, r0
    mov     r0, r5
    mova    id_par2, r0
    mov     r0, r4
    mov.l   @r5, r6
    mov.l   r6, @r4

id_cont1:
    mova    id_par0, r0         ! setup disctype.
    mov     r0, r4
    bsr     GdChangeDataType
    nop

    lds.l   @r15+, pr
    rts
    nop

Init_Error:
    lds.l    @r15+, pr
    rts
    nop

    .align  4
id_m1val:
    .long   1024
id_m2val:
    .long   2048

id_par0:
    .long   0
id_par1:
    .long   8192
id_par2:
    .long   0
id_par3:
    .long   2048

iniparam:
    .long   0, 0, 0, 0

!------------------------------------------------------------------------------
    .align  4
GdReqCmd:
    bra     do_syscall
    mov     #0, r7

GdGetCmdStat:           ! Get Results from buffer
    bra     do_syscall
    mov     #1, r7
    
GdExecServer:           ! Fill buffer with status of current command.
    bra     do_syscall
    mov     #2, r7

GdInitSystem:           ! Initialize GD system.
    bra     do_syscall
    mov     #3, r7

GdGetDrvStat:           ! Get status of drive/media type
    bra     do_syscall
    mov     #4, r7

GdChangeDataType:       ! Prepare GD for GD/CD type.
    mov     #10, r7

do_syscall:
    mov.l   sysvec_bc, r0
    mov     #0, r6
    mov.l   @r0, r0
    jmp     @r0
    nop

    .align  4
sysvec_bc:
    .long   0x8c0000bc

!
! PATCH CODE
!

    .align  4
patch:
    mov.l   patchstart, r6
    mov.l   patchend, r7
    dt      r6
        
patchloop1:                 ! Find the key in memory.
    add     #0x01, r6
    cmp/eq  r6, r7
    bt/s    patchquit
    sub     r0, r0

patchloop2:
    mov.b   @(r0, r6), r1
    mov.b   @(r0, r4), r2
    add     #0x01, r0
    cmp/eq  r1, r2
    bf/s    patchloop1
    cmp/eq  #0x0a, r0
    bf      patchloop2

patchloop3:                 ! Replace with the patch.
    mov.b   @r5+, r1
    mov.b   r1, @r6
    dt      r0
    bf/s    patchloop3
    add     #0x01, r6
    mov     #0x01, r0

patchquit:
    rts
    nop

    .align  4
patchstart:
    .long   0xac000000
patchend:
    .long   0xac003ff9
