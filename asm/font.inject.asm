; a0 - uint8_t *str
; a1 - uint16_t *font_attr
; a2 - uint8_t *font_widths
; a3 - uint16_t glyph_idx
render_font_impl:
	; 음수값 예외처리
	bltz a3, @@_return
	nop

	la s0, is_korean_temp
	lbu s1, 0x0(s0)
	beq s1, zero, @@_is_korean
	nop

	; font_attr[7] = 0
	sh zero, 0xe(a1)

	; font_attr[1] = 0x14
	li s0, 0x14
	sh s0, 0x2(a1)

	; font_attr[9] = 0x12
	li s0, 0x12
	sh s0, 0x12(a1)

	; font_attr[8] = *(a2 + a3)
	addu s0, a2, a3
	lbu s2, 0x0(s0)
	sh s2, 0x10(a1)

	; font_attr[2] = a3 & 1
	andi t0, a3, 0x1
	sh t0, 0x4(a1)

	; s0 = (a3 / 0x12) * 0x12
	; font_attr[4] = s0
	li t0, 0x12
	div a3, t0
	mflo t2
	mfhi v0
	mult s0, t0, t2
	sh s0, 0x8(a1)

	; s1 = ((a3 % 0x12) / 2) * 0xe
	; font_attr[3] = s1
	srl t2, v0, 0x1f
	addu v0, v0, t2
	sra v0, v0, 0x1
	li t1, 0xe
	mult s1, v0, t1
	sh s1, 0x6(a1)

	; font_attr[5] = *(a2 + a3) + s1
	addu s3, s2, s1
	sh s3, 0xa(a1)

	; font_attr[6] = s0 + 0x12
	addiu s0, s0, 0x12
	sh s0, 0xc(a1)
	b @@_return
	nop

@@_is_korean:
	jal @_find_table_index
	nop

	li s0, -1
	bne v0, s0, @@_found
	nop

	jal @_trim_check_table
	nop
	jal @_find_table_index
	nop

@@_found:
	move s0, v0
	andi s0, s0, 0x8000
	bne s0, zero, @@_ignore_copy
	nop

	andi v0, v0, 0x7fff
	move s0, v0
	jal @_copy_font_data
	move s2, a3
	jal @_apply_font_gfx
	nop

@@_ignore_copy:
	; *(@_check_table + (v0 * 2)) = a3
	la s0, @_check_table
	li s2, 2
	andi v0, v0, 0x7fff
	mult s1, v0, s2
	add s0, s0, s1
	sh a3, 0x0(s0)

	; 소문자 정렬
	addiu s0, a3, -0x76a
	sltiu s0, s0, 0x1a
	beq s0, zero, @@_is_lower_case
	nop

	li s0, 2
	b @@_done_set_case
	sh s0, 0xe(a1)
	
@@_is_lower_case:
	sh zero, 0xe(a1)

@@_done_set_case:
	; font_attr[1] = 0x14
	li s0, 0x14
	sh s0, 0x2(a1)

	; font_attr[9] = 0x12
	li s0, 0x12
	sh s0, 0x12(a1)

	; font_attr[8] = *(a2 + a3)
	addu s0, a2, a3
	lbu s2, 0x0(s0)
	sh s2, 0x10(a1)

	; font_attr[2] = v0 & 1
	andi t0, v0, 0x1
	sh t0, 0x4(a1)

	; s0 = (v0 / 0x12) * 0x12
	; font_attr[4] = s0
	li t0, 0x12
	div v0, t0
	mflo t2
	mfhi v0
	mult s0, t0, t2
	sh s0, 0x8(a1)

	; s1 = ((v0 % 0x12) / 2) * 0xe
	; font_attr[3] = s1
	srl t2, v0, 0x1f
	addu v0, v0, t2
	sra v0, v0, 0x1
	li t1, 0xe
	mult s1, v0, t1
	sh s1, 0x6(a1)

	; font_attr[5] = *(a2 + v0) + s1
	addu s3, s2, s1
	sh s3, 0xa(a1)

	; font_attr[6] = s0 + 0x12
	addiu s0, s0, 0x12
	sh s0, 0xc(a1)

@@_return:
	move v0, a0
	ld s0, 0x10(sp)
	ld s1, 0x20(sp)
	ld s2, 0x30(sp)
	ld s3, 0x40(sp)
	ld a2, 0x50(sp)
	ld a3, 0x60(sp)
	ld ra, 0x70(sp)
	jr ra
	addiu sp, sp, 0x80


; 폰트 그래픽 반영
@_apply_font_gfx:
	addiu sp, sp, -0xc0
	sd t3, 0x0(sp)
	sd t4, 0x10(sp)
	sd t5, 0x20(sp)
	sd t6, 0x30(sp)
	sd t7, 0x40(sp)
	sd v0, 0x50(sp)
	sd v1, 0x60(sp)
	sd a0, 0x70(sp)
	sd a1, 0x80(sp)
	sd a2, 0x90(sp)
	sd a3, 0xa0(sp)
	sd ra, 0xb0(sp)

	la v0, font_data_buffer_ptr
	lw a0, 0x0(v0)
	li a1, 0x3c00
	li a2, 0x14
	move a3, zero
	move t0, zero
	lh t1, 0xc(v0)
	jal apply_font_texture_dma
	lh t2, 0xe(v0)

	ld t3, 0x0(sp)
	ld t4, 0x10(sp)
	ld t5, 0x20(sp)
	ld t6, 0x30(sp)
	ld t7, 0x40(sp)
	ld v0, 0x50(sp)
	ld v1, 0x60(sp)
	ld a0, 0x70(sp)
	ld a1, 0x80(sp)
	ld a2, 0x90(sp)
	ld a3, 0xa0(sp)
	ld ra, 0xb0(sp)
	jr ra
	addiu sp, sp, 0xc0

; 폰트 그래픽 복사
; s0: dst 인덱스
; s2: src 인덱스
@_copy_font_data:
	addiu sp, sp, -0x100
	sd v0, 0x10(sp)
	sd a1, 0x20(sp)
	sd a2, 0x30(sp)
	sd a3, 0x40(sp)
	sd s0, 0x50(sp)
	sd s1, 0x60(sp)
	sd s2, 0x70(sp)
	sd s3, 0x80(sp)
	sd t0, 0x90(sp)
	sd t1, 0xa0(sp)
	sd t2, 0xb0(sp)
	sd t3, 0xc0(sp)
	sd v1, 0xd0(sp)
	sd a0, 0xe0(sp)
	sd ra, 0xf0(sp)

	; s0 = @_calc_tile_offset(s0).addr
	; s1 = @_calc_tile_offset(s0).is_odd
	jal @_calc_tile_offset
	move a0, s0
	move s0, v0
	move s1, v1

	; s2 = @_calc_tile_offset(s2).addr
	; s3 = @_calc_tile_offset(s2).is_odd
	jal @_calc_tile_offset
	move a0, s2
	move s2, v0
	move s3, v1

	; s2 = font_data_buffer + s0
	la t0, font_data_buffer
	addu s0, t0, s0

	; s2 = font_data + s2
	la t0, font_data
	addu s2, t0, s2

	; s1 != 0
	bne s1, zero, @@_s1_odd
	nop

	li t0, 0xcc
	beq s3, zero, @@_s1_even_s3_even
	nop

	li t1, 0xcc
	b @@_start_copy
	li s3, 2

@@_s1_even_s3_even:
	li t1, 0x33
	b @@_start_copy
	li s3, 0

@@_s1_odd:
	li t0, 0x33
	bne s3, zero, @@_s1_odd_s3_even
	nop

	li t1, 0x33
	b @@_start_copy
	li s3, 2

@@_s1_odd_s3_even:
	li t1, 0xcc
	li s3, 0

@@_start_copy:
	li a1, 0
@@_for_0x12_loop:
	li a2, 0
	li t2, 0x12
	beq a1, t2, @@_end_for_0x12
	nop
@@_for_7_loop:
	li t2, 7
	beq a2, t2, @@_end_for_7
	nop

	lbu t2, 0x0(s0)
	and t2, t2, t0

	lbu t3, 0x0(s2)
	and t3, t3, t1

	beq s1, zero, @@_s1_is_even
	nop

	sllv t3, t3, s3
	b @@_store_t2
	or t2, t2, t3

@@_s1_is_even:
	srlv t3, t3, s3
	or t2, t2, t3

@@_store_t2:
	sb t2, 0x0(s0)

	addi s0, s0, 1
	addi s2, s2, 1
	b @@_for_7_loop
	addi a2, a2, 1
@@_end_for_7:
	addi s0, s0, 0x40 - 7
	addi s2, s2, 0x40 - 7
	b @@_for_0x12_loop
	addi a1, a1, 1

@@_end_for_0x12:
	ld v0, 0x10(sp)
	ld a1, 0x20(sp)
	ld a2, 0x30(sp)
	ld a3, 0x40(sp)
	ld s0, 0x50(sp)
	ld s1, 0x60(sp)
	ld s2, 0x70(sp)
	ld s3, 0x80(sp)
	ld t0, 0x90(sp)
	ld t1, 0xa0(sp)
	ld t2, 0xb0(sp)
	ld t3, 0xc0(sp)
	ld v1, 0xd0(sp)
	ld a0, 0xe0(sp)
	ld ra, 0xf0(sp)
	jr ra
	addiu sp, sp, 0x100


; 복사 오프셋 계산
; from a0: index
; to v0: 변환 오프셋
;    v1: 홀수 유무
@_calc_tile_offset:
	addiu sp, sp, -0x50
	sd s0, 0x10(sp)
	sd s1, 0x20(sp)
	sd s2, 0x30(sp)
	sd s3, 0x40(sp)

	; s0 = a0 / 0x12
	; s1 = (a0 % 0x12) / 2
	li t0, 0x12
	div a0, t0
	mflo s0
	mfhi s1
	sra s1, s1, 1

	; v0 = (s0 * 0x480) + (s1 * 7)
	li t0, 0x480
	mult s2, s0, t0
	li t0, 7
	mult s3, s1, t0
	addu v0, s2, s3

	; v1 = a0 & 1
	andi v1, a0, 1

	ld s0, 0x10(sp)
	ld s1, 0x20(sp)
	ld s2, 0x30(sp)
	ld s3, 0x40(sp)
	jr ra
	addiu sp, sp, 0x50


; 검사 테이블 정리
@_trim_check_table:
	la s0, @_check_table
	li s1, 0
	li v0, 0xffff

@@_loop:
	bge s1, TEXTURE_GLYPH_COUNT, @@_end_loop
	nop

	sh v0, 0x0(s0)
	addiu s0, s0, 2
	j @@_loop
	addiu s1, s1, 1

@@_end_loop:
	la a0, font_data_buffer
	li a1, 0

@@_loop_fill_zero:
	bge a1, 4032, @@_end_loop_fill_zero
	nop

	sq zero, 0(a0)
	addiu a0, a0, 16
	b @@_loop_fill_zero
	addiu a1, a1, 1

@@_end_loop_fill_zero:
	jr ra
	nop


; 폰트 인덱스 찾기
; v0
;   -2: 인덱스 테이블 비워야함.
;   정수값: 인덱스
@_find_table_index:
	la s0, @_check_table
	li s1, 0
	li v0, 0xffff

@@_loop:
	bge s1, TEXTURE_GLYPH_COUNT, @@_failure
	nop

	; *s0 == a3
	lhu s2, 0x0(s0)
	beq s2, a3, @@_found_ignore_copy
	nop

	; *s0 == 0xffff
	beq s2, v0, @@_found
	nop

@@_continue:
	addiu s0, s0, 2
	addiu s1, s1, 1
	j @@_loop
	nop

@@_found_ignore_copy:
	ori s1, s1, 0x8000
	jr ra
	move v0, s1

@@_found:
	jr ra
	move v0, s1

@@_failure:
	jr ra
	li v0, -1


.align 4
@_check_table:
	.fill TEXTURE_GLYPH_COUNT * 2, 0xff

.align 4
font_data:
	.incbin "build/files/file_00455.data.bin"

.align 4
font_width_table:
	.incbin "build/files/file_00455.width.bin"
