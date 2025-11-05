; 필드용 적 이름 로딩
get_monster_name_by_id_internal:
	addiu sp, sp, -0x10
	sd ra, 0x0(sp)

	jal get_language_config
	nop
	beq v0, zero, @@_is_korean
	nop

	li v0, 472
	lui v1, 0x31
	mult a0, a0, v0
	lw v0, -0xc0(v1)
	addu v0, v0, a0
	b @@_return
	addiu v0, v0, 408

@@_is_korean:
	la v0, monster_names
	li v1, 24
	mult a0, a0, v1
	addu v0, v0, a0

@@_return:
	ld ra, 0x0(sp)
	jr ra
	addiu sp, sp, 0x10


; 적의 상태이상 배경 그리기 & 텍스트 출력
draw_battle_scan_status:
	li a3, 0x16
	move a1, s1
	jal draw_scan_status_background
	li t0, 5
	addiu s1, s1, -2
	la t0, return_lab_00218ec8
	jr t0
	move a0, s3
