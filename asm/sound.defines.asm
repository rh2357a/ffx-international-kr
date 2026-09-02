; FFX International only. Persistent memory is outside the font/code image.
SHOUT_BANK          equ 0xa00000
SHOUT_BANK_CAPACITY equ 0xa0000
SHOUT_STAGE         equ 0xaa0000
SHOUT_STAGE_SIZE    equ 0x8000
SHOUT_STATE         equ 0x7d7000
SHOUT_RESIDENT_END  equ 0x7d7428
SHOUT_POOL_START    equ 0xab0000
SHOUT_IOP_TAG       equ 0x53480000
SHOUT_IOP_RPC       equ 19
SHOUT_IOP_MAGIC     equ 0x5348
SHOUT_COUNT         equ 45
SHOUT_DATA_OFFSET   equ 0xc0
.if SHOUT_RESIDENT_END > 0x7f0000
    .error "Resident data overlaps the main stack"
.endif
.if SHOUT_STATE + 0x10 > SHOUT_RESIDENT_END
    .error "SHOUT state must remain in cleared BSS"
.endif
.if SHOUT_BANK < 0xa00000
    .error "SHOUT bank overlaps stack or graphics MFIFO"
.endif
.if SHOUT_BANK + SHOUT_BANK_CAPACITY > SHOUT_STAGE
    .error "SHOUT bank overlaps staging memory"
.endif
.if SHOUT_STAGE + SHOUT_STAGE_SIZE > SHOUT_POOL_START
    .error "SHOUT staging memory overlaps game allocator"
.endif
.if (SHOUT_POOL_START & 0xffff) != 0
    .error "Game pool start must fit the existing LUI instructions"
.endif
; State: +0 bank size, +4 last error, +8 busy, +12 successful starts.
.definelabel shout_file_size, 0x125460
.definelabel shout_load_file, 0x1250e8
.definelabel shout_free_file, 0x124788
.definelabel shout_rpc_call, 0x2512d0
.definelabel shout_send_bank, 0x2538e0
.definelabel shout_wait_dma, 0x2519d8
