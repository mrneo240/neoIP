.text
.global _start2
.type _start2, @function

_start2:
    mov.l   boot2, r0
    jsr     @r0
    nop

    .align  4
boot2:
    .long   0x8c00e000
