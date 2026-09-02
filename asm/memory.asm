; .bss 확장

.org 0x100008
	la v0, 0x57ea00
	la v1, 0x797428 + 0x40000

.org 0x10006c
	la a0, 0x797428 + 0x40000
	lui a1, 0x0
	addiu a1, a1, -0x1
	li v1, 0x3d
	syscall 0x0

.org 0x56a4f4
	dw 0x797428 + 0x40000

.orga 0x484d7c
	dw 0x2189a8 + 0x40000

; Keep the main stack and graphics MFIFO in their original ranges.
; Carve SHOUT memory out of the start of game arena 0 (normal/debug paths).
.if MULTILANG == 0
.org 0x11be8c
	lui a1, SHOUT_POOL_START >> 16
.org 0x11beb8
	lui a1, SHOUT_POOL_START >> 16
.endif
