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

