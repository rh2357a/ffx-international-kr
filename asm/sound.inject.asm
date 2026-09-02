.align 4
; Preserve the unmodified bank in EE RAM. Never DMA the complete bank to IOP.
shout_init:
    addiu sp, sp, -0x30
    sd ra, 0x20(sp)
    sd s0, 0(sp)
    sd s1, 0x10(sp)
    la s1, SHOUT_STATE
    sw zero, 0(s1)
    sw zero, 4(s1)
    sw zero, 8(s1)
    sw zero, 12(s1)
    li a0, 4
    move a1, zero
    move a2, zero
    jal shout_rpc
    move a3, zero
    li t0, SHOUT_IOP_MAGIC
    bne v0, t0, @@bad_module
    nop
    li a0, 24
    jal shout_file_size
    li a1, 5
    move s0, v0
    sltiu t0, s0, 0xb4 + 0x800
    bnez t0, @@bad_size
    li t1, SHOUT_BANK_CAPACITY + 1
    sltu t0, s0, t1
    beqz t0, @@bad_size
    li a0, 24
    jal shout_load_file
    li a1, 5
    beqz v0, @@bad_size
    nop
    sd v0, 0x18(sp)
    move a1, v0
    la a0, SHOUT_BANK
    jal memcpy
    move a2, s0
    ld a0, 0x18(sp)
    jal shout_free_file
    nop
    la t0, SHOUT_BANK
    lw t1, 0(t0)
    li t2, 0xb4
    bne t1, t2, @@bad_size
    nop
    sw s0, 0(s1)
    b @@done
    nop
@@bad_module:
    li t0, 1
    b @@error
    nop
@@bad_size:
    li t0, 2
@@error:
    sw t0, 4(s1)
@@done:
    ld ra, 0x20(sp)
    ld s1, 0x10(sp)
    ld s0, 0(sp)
    jr ra
    addiu sp, sp, 0x30

; a0 selector; a1/a2/a3 target arguments. Blocking RPC returns target v0.
; Tagged RPC 19 requests; original untagged requests retain no-op behavior.
shout_rpc:
    addiu sp, sp, -0x10
    sd ra, 0(sp)
    la t0, 0x328e98
    lw t0, 0(t0)
    li t1, SHOUT_IOP_TAG
    or t1, t1, a0
    sw t1, 0(t0)
    sw a1, 4(t0)
    sw a2, 8(t0)
    sw a3, 12(t0)
    li a0, SHOUT_IOP_RPC
    jal shout_rpc_call
    li a1, 1
    la t0, 0x328ea0
    lw t0, 0(t0)
    lw v0, 0(t0)
    ld ra, 0(sp)
    jr ra
    addiu sp, sp, 0x10

; Same two SPU DMA conditions checked by the original SHOUT worker.
; Polling is bounded; on failure the caller keeps the old IOP bytes intact.
shout_quiesce:
    addiu sp, sp, -0x30
    sd ra, 0x20(sp)
    sd s0, 0(sp)
    sd s1, 0x10(sp)
    li s0, 256
@@retry:
    move s1, zero
@@core:
    li a0, 2
    move a1, s1
    move a2, zero
    jal shout_rpc
    move a3, zero
    bnez v0, @@busy
    nop
    li a0, 3
    move a1, s1
    move a2, zero
    jal shout_rpc
    move a3, zero
    blez v0, @@busy
    nop
    addiu s1, s1, 1
    sltiu t0, s1, 2
    bnez t0, @@core
    nop
    b @@done
    li v0, 1
@@busy:
    addiu s0, s0, -1
    bnez s0, @@retry
    nop
    move v0, zero
@@done:
    ld ra, 0x20(sp)
    ld s1, 0x10(sp)
    ld s0, 0(sp)
    jr ra
    addiu sp, sp, 0x30

shout_play:
    addiu sp, sp, -0x40
    sd ra, 0x30(sp)
    sd s0, 0(sp)
    sd s1, 0x10(sp)
    sd s2, 0x20(sp)
    la s2, SHOUT_STATE
    lw t0, 8(s2)
    bnez t0, @@return
    move v0, zero
    li t0, 1
    sw t0, 8(s2)
    sw zero, 4(s2)
    move s0, a0
    sltiu t0, s0, SHOUT_COUNT
    beqz t0, @@bad_clip
    nop
    la t0, SHOUT_BANK
    sll t1, s0, 2
    addu t1, t0, t1
    lw t1, 0(t1)
    sltiu t2, t1, 0xb4
    bnez t2, @@bad_clip
    addiu t2, t1, -0xb4
    andi t2, t2, 0x7ff
    bnez t2, @@bad_clip
    lw t3, 0(s2)
    addiu t3, t3, -0x20
    sltu t2, t3, t1
    bnez t2, @@bad_clip
    addu t0, t0, t1
    lw t2, 0(t0)
    li t3, 0x5356
    bne t2, t3, @@bad_clip
    lw t2, 4(t0)
    bnez t2, @@bad_clip
    lw t2, 8(t0)
    bnez t2, @@bad_clip
    lw t2, 12(t0)
    addiu t3, t2, -1
    sltiu t3, t3, (SHOUT_STAGE_SIZE - SHOUT_DATA_OFFSET) / 0x800
    beqz t3, @@bad_clip
    sll s1, t2, 11
    addu t2, t1, s1
    lw t3, 0(s2)
    sltu t3, t3, t2
    bnez t3, @@bad_clip
    nop
    ; Require the next offset to match the VS block count too.
    sltiu t3, s0, SHOUT_COUNT - 1
    beqz t3, @@copy
    la t3, SHOUT_BANK
    sll t4, s0, 2
    addu t3, t3, t4
    lw t3, 4(t3)
    bne t2, t3, @@bad_clip
    nop
@@copy:
    move a1, t0
    la a0, SHOUT_STAGE + SHOUT_DATA_OFFSET
    jal memcpy
    move a2, s1
    la t0, SHOUT_STAGE
    li t1, SHOUT_COUNT
    li t2, SHOUT_DATA_OFFSET
@@table:
    sw t2, 0(t0)
    addiu t1, t1, -1
    bnez t1, @@table
    addiu t0, t0, 4
    sw zero, 0(t0)
    sw zero, 4(t0)
    sw zero, 8(t0)
    jal shout_quiesce
    nop
    beqz v0, @@timeout
    nop
    ; Stop SHOUT/core 0 only, including its normal status notification.
    move a0, zero
    li a1, 1
    move a2, zero
    jal shout_rpc
    move a3, zero
    la t0, 0x69dd94
    sw zero, 0(t0)
    jal shout_quiesce
    nop
    beqz v0, @@timeout
    nop
    ; Serialize against any previous use of the shared SIF DMA descriptor.
    jal shout_wait_dma
    li a0, 1
    la a0, SHOUT_STAGE
    jal shout_send_bank
    addiu a1, s1, SHOUT_DATA_OFFSET
    jal shout_wait_dma
    li a0, 1
    ; Original SHOUT defaults: pan 0x40, volume 0x7f, original clip ID.
    ; Direct synchronous start avoids a pending worker retaining old data.
    li s1, 256
@@start_retry:
    jal shout_quiesce
    nop
    beqz v0, @@timeout
    nop
    li a0, 1
    move a1, s0
    li a2, 0x40
    jal shout_rpc
    li a3, 0x7f
    bgez v0, @@started
    nop
    addiu s1, s1, -1
    bnez s1, @@start_retry
    nop
    b @@start_failed
    nop
@@started:
    la t0, 0x69e73c
    li t1, 1
    sw t1, 0(t0)
    la t0, 0x69dd94
    sw t1, 0(t0)
    lw t0, 12(s2)
    addiu t0, t0, 1
    sw t0, 12(s2)
    b @@unlock
    li v0, 1
@@bad_clip:
    li t0, 3
    b @@error
    nop
@@timeout:
    li t0, 4
    b @@error
    nop
@@start_failed:
    li t0, 5
@@error:
    sw t0, 4(s2)
    move v0, zero
@@unlock:
    sw zero, 8(s2)
@@return:
    ld ra, 0x30(sp)
    ld s2, 0x20(sp)
    ld s1, 0x10(sp)
    ld s0, 0(sp)
    jr ra
    addiu sp, sp, 0x40
