; 폰트 파일을 통한 코드 주입 + 폰트 데이터 버퍼 설정
.org 0x20c3c0
.area 44, 0
	la a0, inject_begin
	lw a2, 0x24(s1)
	jal memcpy
	addu a1, s1, a1

	la s0, font_data_buffer
	sw s0, 0x0(s2)

	jal flush_cache
	li a0, 2
.endarea


; 폰트폭 주소 설정
.org 0x20c44c
.area 16, 0
	la v1, font_width_table
	sll a0, a1, 4
	addu v0, v0, a0
.endarea


; FUN_00206fc0: placeholder 문자열 길이 관련
.org 0x20702c
	sltiu v0, v0, MULTIBYTE_BASE


; FUN_002053c0: 문자 너비 관련?
.org 0x2053c0
read_glyph_width:
.area 496, 0
	addiu sp, sp, -0x40
	sd s0, 0x0(sp)
	sd s1, 0x10(sp)
	sd s2, 0x20(sp)
	sd ra, 0x30(sp)

	move s2, a2
	move s0, a0
	lbu a0, 0x0(s0)

	sltiu v0, a0, 0x30
	bne v0, zero, @@_under_0x30
	move s1, a1

	jal get_font_width_ptr
	move a0, a3

	lbu v1, 0x0(s0)
	addu v0, v1, v0
	move a0, v1
	b @@_end_read_width
	lb t0, -0x30(v0)

@@_under_0x30:
	sltiu v0, a0, MULTIBYTE_BASE
	bne v0, zero, @@_end_read_width
	nop

	jal get_font_width_ptr
	move a0, zero

	lbu a0, 0x0(s0)
	li a1, 0xd0
	lbu a2, 0x1(s0)
	addiu v1, a0, -(MULTIBYTE_BASE - 1)
	mult v1, v1, a1
	addiu v1, v1, -0x30
	addu v1, v1, a2
	addu v0, v0, v1
	b @@_end_read_width
	lb t0, 0x0(v0)

@@_end_read_width:
	addiu v0, t0, 2
	li v1, 1
	beq s1, v1, @@_1
	movz t0, v0, s1

	li v0, 3
	bnel s1, v0, @@_2
	sw t0, 0x0(s2)

@@_1:
	addiu t0, t0, 4
	sw t0, 0x0(s2)

@@_2:
	lbu v0, 0x0(s0)
	sltiu v0, v0, 0x30
	bnel v0, zero, @@_3
	addiu s0, s0, 2
	addiu s0, s0, 1

@@_3:
	move v0, s0
	ld ra, 0x30(sp)
	ld s2, 0x20(sp)
	ld s1, 0x10(sp)
	ld s0, 0x0(sp)
	jr ra
	addiu sp, sp, 0x40
.endarea


; FUN_00204d70: 폰트 그래픽
.org 0x204d70
render_font:
.area 784, 0
	addiu sp, sp, -0x80
	sd s0, 0x10(sp)
	sd s1, 0x20(sp)
	sd s2, 0x30(sp)
	sd s3, 0x40(sp)
	sd a2, 0x50(sp)
	sd a3, 0x60(sp)
	sd ra, 0x70(sp)

	; *a0 < 0x24
	lbu v1, 0x0(a0)
	sltiu v0, v1, MULTIBYTE_BASE
	beq v0, zero, @@_valid
	nop

@@_return:
	ld s0, 0x10(sp)
	ld s1, 0x20(sp)
	ld s2, 0x30(sp)
	ld s3, 0x40(sp)
	ld a2, 0x50(sp)
	ld a3, 0x60(sp)
	ld ra, 0x70(sp)
	jr ra
	addiu sp, sp, 0x80

@@_valid:
	move s1, a0

	; *0x3257d0 != 1
	la v0, 0x3257d0
	lw v1, 0x0(v0)
	li v0, 0x1
	bne v1, v0, @@_is_korean
	nop

	; get_language_config() == 0
	jal get_language_config
	nop
	beq v0, zero, @@_is_korean
	nop

	li s0, 0x3d40
	sh s0, 0x0(a1)
	b @@_glyph_idx
	nop

@@_is_korean:
	li s0, 0x3c00
	sh s0, 0x0(a1)

@@_glyph_idx:
	jal get_font_locale_config
	nop

	la a2, is_korean_temp
	sb v0, 0x0(a2)

	jal get_font_width_ptr
	move a0, v0

	move a2, v0
	move a0, s1
	li a3, 0

	; *a3 < 0x30
	lbu v0, 0x0(a0)
	sltiu v0, v0, 0x30
	beq v0, zero, @@_not_multibyte
	nop

	; a3 = (*a0 - 0x23) * 0xd0
	; a0++;
	lbu a3, 0x0(a0)
	addiu a3, a3, -(MULTIBYTE_BASE - 1)
	li v1, 0xd0
	mult a3, a3, v1
	addiu a0, a0, 0x1

@@_not_multibyte:
	; a3 = a3 - 0x30 + *a3++
	lbu s1, 0x0(a0)
	addiu a3, a3, -0x30
	addu a3, a3, s1
	addiu a0, a0, 0x1

	la s0, render_font_impl
	jr s0
	nop
.endarea


; FUN_00205080: 폰트 그래픽 (언어 설정 강제)
.org 0x205080
render_font_2:
.area 624, 0
	addiu sp, sp, -0x80
	sd s0, 0x10(sp)
	sd s1, 0x20(sp)
	sd s2, 0x30(sp)
	sd s3, 0x40(sp)
	sd a2, 0x50(sp)
	sd a3, 0x60(sp)
	sd ra, 0x70(sp)

	; *a0 < 0x24
	lbu v1, 0x0(a0)
	sltiu v0, v1, MULTIBYTE_BASE
	beq v0, zero, @@_valid
	nop

@@_return:
	ld s0, 0x10(sp)
	ld s1, 0x20(sp)
	ld s2, 0x30(sp)
	ld s3, 0x40(sp)
	ld a2, 0x50(sp)
	ld a3, 0x60(sp)
	ld ra, 0x70(sp)
	jr ra
	addiu sp, sp, 0x80

@@_valid:
	move s1, a0

	; a2 == 0
	beq a2, zero, @@_is_korean
	nop

	li a0, 0x4
	li s0, 0x3d40
	sh s0, 0x0(a1)
	b @@_glyph_idx
	nop

@@_is_korean:
	li a0, 0x0
	li s0, 0x3c00
	sh s0, 0x0(a1)

@@_glyph_idx:
	la a2, is_korean_temp
	sb a0, 0x0(a2)

	jal get_font_width_ptr
	nop

	move a2, v0
	move a0, s1
	li a3, 0

	; *a3 < 0x30
	lbu v0, 0x0(a0)
	sltiu v0, v0, 0x30
	beq v0, zero, @@_not_multibyte
	nop

	; a3 = (*a0 - 0x23) * 0xd0
	; a0++;
	lbu a3, 0x0(a0)
	addiu a3, a3, -(MULTIBYTE_BASE - 1)
	li v1, 0xd0
	mult a3, a3, v1
	addiu a0, a0, 0x1

@@_not_multibyte:
	; a3 = a3 - 0x30 + *a3++
	lbu s1, 0x0(a0)
	addiu a3, a3, -0x30
	addu a3, a3, s1
	addiu a0, a0, 0x1

	la s0, render_font_impl
	jr s0
	nop
.endarea
