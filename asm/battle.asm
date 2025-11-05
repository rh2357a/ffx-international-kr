; LAB_0016ebe8: 몬스터 이름 포인터 반환
.org 0x16ebe8
get_monster_name_by_id:
.area 32, 0
	la v0, get_monster_name_by_id_internal
	jr v0
	nop
.endarea


; 아군 상태이상 텍스트 폰트 변경
.org 0x219934
	jal render_battle_text_en
	addu a1, s2, a1


; 라이브라: 적의 상태이상 텍스트 폰트 변경
.org 0x21d060
	jal render_battle_text_en
	lh a2, 0x72(sp)
.org 0x218c24
	addiu v0, s0, 0x18
.org 0x218e94
.area 28, 0
	la a1, draw_battle_scan_status
	jr a1
	li a2, 0xa3
.endarea
.org 0x218ec8
return_lab_00218ec8:
	; ...


; 아군 한글 이름 출력 (버그 수정)
.macro fix_battle_name_bug, offsets
.org offsets
.area 164, 0
	jal FUN_001bb1d0
	lhu a0, 0x0(s7)

	lw a2, 0xc8(sp)
	move a1, v0
	lw a3, 0xcc(sp)
	move a0, zero
	move t0, zero
	jal FUN_00206080
	move t1, zero

	jal FUN_001bb1d0
	lhu a0, 0x0(s7)

	lh v1, 0x30(sp)
	move a0, v0
	lh a1, 0xc0(sp)
	lh a2, 0x32(sp)
	lbu a3, 0x19(s6)

	; 출력 위치 조정 (y)
	addiu a2, a2, -2

	jal FUN_001f3850
	subu a1, v1, a1

	lh a1, 0x60(sp)
.endarea
.endmacro

.if MULTILANG == 0
	fix_battle_name_bug 0x21c8dc
	fix_battle_name_bug 0x21cb08
	fix_battle_name_bug 0x21cd10
.endif

; 아군 한글 이름 출력 (버그 수정 - 2)
.if MULTILANG == 0
.org 0x21cf28
.area 164, 0
	jal FUN_001bb1d0
	lhu a0, 0x0(s7)

	lw a2, 0xc8(sp)
	move a1, v0
	lw a3, 0xcc(sp)
	move a0, zero
	move t0, zero
	jal FUN_00206080
	move t1, zero

	jal FUN_001bb1d0
	lhu a0,0x0(s7)

	lh v1, 0x30(sp)
	move a0, v0
	lh a1, 0xc0(sp)
	lh a2, 0x32(sp)
	lbu a3, 0x19(s6)

	; 출력 위치 조정 (y)
	addiu a2, a2, -2

	jal FUN_001f3850
	subu a1, v1, a1

	lh a0, 0x40(s6)
.endarea
.endif
