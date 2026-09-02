; Original International file_00004.elf only (.text file offset = VA + 0xa0).
; RPC 19's original calls must remain no-ops. Only requests tagged with
; SHOUT_IOP_TAG reach this helper. RPC 20 still returns zero without printf.
; Every relocated instruction/data address below has a matching relocation.
.psx
.open "build/" + TARGET_NAME + "/files/file_00004.elf", 0
.orga 0xa0
.headersize -0xa0
.org 0x19060
    dw 0x12a08
.org 0x19064
    dw 0xff00

.org 0x12a08
.area 0x74, 0
iop_shout_rpc:
    addiu sp, sp, -0x18
    sw ra, 0x10(sp)
    lw t0, 0(a0)
    lui t1, SHOUT_IOP_TAG >> 16
    subu t0, t0, t1
    sltiu t1, t0, 5
    beqz t1, iop_shout_done
    move v0, zero
    li v0, SHOUT_IOP_MAGIC
    sltiu t1, t0, 4
    beqz t1, iop_shout_done
    sll t0, t0, 2
iop_shout_hi:
    lui t9, (iop_shout_functions + 0x8000) >> 16
iop_shout_lo:
    addiu t9, t9, iop_shout_functions & 0xffff
    addu t9, t9, t0
    lw t9, 0(t9)
    lw a2, 0xc(a0)
    lw a1, 8(a0)
    lw a0, 4(a0)
    jalr t9
    nop
iop_shout_done:
    lw ra, 0x10(sp)
    nop
    jr ra
    addiu sp, sp, 0x18
iop_shout_functions:
    dw 0xc938, 0xe6d0, 0xfe3c, 0x15d7c
.endarea

; Six replacement relocations. Move the independent R_MIPS_26 record from
; the tail into the seventh slot and compact the final HI16/LO16 pair.
; This removes one record without introducing a relocation into unused code.
.orga 0x20328
    dw iop_shout_hi, 5
    dw iop_shout_lo, 6
    dw iop_shout_functions + 0, 2
    dw iop_shout_functions + 4, 2
    dw iop_shout_functions + 8, 2
    dw iop_shout_functions + 12, 2
    dw 0x15cec, 4
.orga 0x21ee8
    dw 0x15cf4, 5
    dw 0x15cf8, 6
.orga 0x19474
    dw 0x8930
.close
.ps2
