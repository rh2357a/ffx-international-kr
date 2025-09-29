; LAB_0016ebe8: 몬스터 이름 포인터 반환
.org 0x16ebe8
get_monster_name_by_id:
.area 32, 0
	la v0, monster_names
	li v1, 20
	mult a0, a0, v1
	jr ra
	addu v0, v0, a0
.endarea


;	; v0 = (0x310000 - 0xc0) + (a0 * 472)
;	li v0, 472
;	lui v1, 0x31
;	mult a0, a0, v0
;	lw v0, -0xc0(v1)
;	addu v0, v0, a0
;
;	; return v0 + 408
;	jr ra
;	addiu v0, v0, 410
