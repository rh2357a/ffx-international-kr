; s1: str
; s0: attr
albhed_to_korean:
	addiu sp, sp, -0x30
	sd t0, 0x0(sp)
	sd t1, 0x10(sp)
	sd t2, 0x20(sp)

	; v0 = *s1
	; if (!(v0 >= 0x24 && v0 <= 0x2f))
	lbu v0, 0x0(s1)
	addi t0, v0, -0x24
	sltiu t1, t0, 0xc
	bne t1, zero, @@_korean
	nop

	; 기존 일어 코드
	sltiu t0, v0, 0x30
	beq t0, zero, @@_end
	sb v0, 0x0(s0)
	lbu v0, 0x1(s1)
	addiu s0, s0, 1
	b @@_end
	sb v0, 0x0(s0)

@@_korean:
	; t0 = (*s1 - MULTIBYTE_BASE) * 0xd0
	; t0 = (*(s1 + 1) - 0x30) + t0
	addiu t0, v0, -MULTIBYTE_BASE
	li t1, 0xd0
	mult t0, t0, t1
	lbu t1, 0x1(s1)
	addiu t1, t1, -0x30
	addu t0, t0, t1

	; t0 *= 8
	li t2, 8
	mult t0, t0, t2

	; t1 = albhed_to_ko_table + t0
	la t1, albhed_to_ko_table
	addu t1, t1, t0

	li v0, 0
	addi s0, s0, 1

	; 자음
	lui v1, 0x32
	lw v1, -0xfa0(v1)
	lbu t0, 0x0(t1)
	srav v1, v1, t0
	andi v1, v1, 1
	beq v1, zero, @@_translate_moum
	nop

	addi v0, v0, 1

	; 모음
@@_translate_moum:
	lui v1, 0x32
	lw v1, -0xfa0(v1)
	lbu t0, 0x1(t1)
	srav v1, v1, t0
	andi v1, v1, 1
	beq v1, zero, @@_translate_result
	nop

	addi v0, v0, 2

@@_translate_result:
	beq v0, zero, @@_0
	nop

	li v1, 1
	beq v0, v1, @@_1
	nop

	addi v1, v1, 1
	beq v0, v1, @@_2
	nop

	b @@_3
	nop

@@_0:
	lbu t0, 0x2(t1)
	sb t0, -0x1(s0)
	lbu t0, 0x3(t1)
	sb t0, 0x0(s0)
	b @@_return
	li v0, ALBHED_UNTRANSLATED_COLOR

@@_1:
	lbu t0, 0x4(t1)
	sb t0, -0x1(s0)
	lbu t0, 0x5(t1)
	sb t0, 0x0(s0)
	b @@_return
	li v0, ALBHED_UNTRANSLATED_COLOR_2

@@_2:
	lbu t0, 0x6(t1)
	sb t0, -0x1(s0)
	lbu t0, 0x7(t1)
	sb t0, 0x0(s0)
	b @@_return
	li v0, ALBHED_UNTRANSLATED_COLOR_2

@@_3:
	lbu t0, 0x0(s1)
	sb t0, -0x1(s0)
	lbu t0, 0x1(s1)
	sb t0, 0x0(s0)
	b @@_return
	li v0, ALBHED_TRANSLATED_COLOR

@@_end:
	; s0[1] = 0
	sb zero, 0x1(s0)

	; v0 = *DAT_0031f060 & 0x3ffffff
	li v1, 0x3ffffff
	la a0, 0x31f060
	lw v0, 0x0(a0)
	xor v0, v0, v1
	beq v0, zero, @@_return
	li v0, ALBHED_TRANSLATED_COLOR

	li v0, ALBHED_UNTRANSLATED_COLOR

@@_return:
	ld t0, 0x0(sp)
	ld t1, 0x10(sp)
	ld t2, 0x20(sp)
	addiu sp, sp, 0x30

	; FUN_002022c0의 스택 복구
	ld ra, 0x20(sp)
	ld s1, 0x10(sp)
	ld s0, 0x0(sp)
	jr ra
	addiu sp, sp, 0x30
