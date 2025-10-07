; LAB_0016ebe8: 몬스터 이름 포인터 반환
.org 0x16ebe8
get_monster_name_by_id:
.area 32, 0
	la v0, get_monster_name_by_id_internal
	jr v0
	nop
.endarea
