; Tail of FUN_001bddc0. s0 = name index, v0 = slot pointer (or null).
; Reuse its 0x20-byte frame and return the original pointer unchanged.
.align 4
repair_empty_rikku_name:
	beq v0, zero, @@_return
	li t0, 6
	bne s0, t0, @@_return
	nop
	lbu t0, 0(v0)
	bne t0, zero, @@_return
	nop

	la t0, @@_default_name
	move t1, v0
@@_copy:
	lbu t2, 0(t0)
	addiu t0, t0, 1
	sb t2, 0(t1)
	bne t2, zero, @@_copy
	addiu t1, t1, 1

@@_return:
	ld ra, 0x10(sp)
	ld s0, 0(sp)
	jr ra
	addiu sp, sp, 0x20

@@_default_name:
.if MULTILANG == 0
	.string "류크"
.else
	; "Rikku"
	.db 0x61, 0x78, 0x7a, 0x7a, 0x84, 0
.endif
.align 4
