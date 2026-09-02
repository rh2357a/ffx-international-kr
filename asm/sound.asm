; Install the existing font injection at sound initialization as well.
; SHOUT is initialized before the usual font loader, so a direct hook into
; font-resident code would otherwise execute uninitialized memory.
.org 0x1256f0
.area 0x98, 0
    addiu sp, sp, -0x20
    sd ra, 0x10(sp)
    sd s0, 0(sp)
    li a0, 18
    jal shout_load_file
    li a1, 0
    beqz v0, @@done
    move s0, v0
    lw a2, 0x24(s0)
    li t0, inject_end - inject_begin + 0x40
    bne a2, t0, @@bad_font
    nop
    la a0, inject_begin
    addiu a1, s0, 0x40
    jal memcpy
    addiu a2, a2, -0x40
    jal flush_cache
    move a0, zero
    jal flush_cache
    li a0, 2
    jal shout_free_file
    move a0, s0
    jal shout_init
    nop
    b @@done
    nop
@@bad_font:
    jal shout_free_file
    move a0, s0
@@done:
    ld ra, 0x10(sp)
    ld s0, 0(sp)
    jr ra
    addiu sp, sp, 0x20
.endarea

; Guard resides in the ELF and is safe even if initialization failed.
; All original calls to the SHOUT start wrapper retain their entry address.
.org 0x2539b8
.area 0x50, 0
    la t0, SHOUT_STATE
    lw t0, 0(t0)
    beqz t0, @@unavailable
    nop
    j shout_play
    nop
@@unavailable:
    jr ra
    move v0, zero
.endarea
