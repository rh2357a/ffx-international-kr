.loadtable "tables/armips.tbl"


.ps2
.open "build/files/file_00000.elf", 0xff000
.include "asm/defines.asm"
.include "asm/macros.asm"
.close


.ps2
.open "build/files/file_00455.ftcx", 0x798000 - 0x40

.orga 0x40
inject_begin:
.area INJECT_LENGTH, 0
.include "asm/font.inject.asm"
.include "texts/battle2/mon_names.asm"
.include "asm/albhed.inject.asm"
.endarea

.orga 0x40 + INJECT_LENGTH
font_data:
	; .fill 0x2a780, 0

.orga 0x40 + INJECT_LENGTH + 0x2a780
font_width_table:
	; .fill 2704, 0

.close


.ps2
.open "build/files/file_00000.elf", 0xff000
.include "asm/region.asm"
.include "asm/albhed.asm"
.include "asm/airship_naming.asm"
.include "asm/battle.asm"
.include "asm/font.asm"
.close


; .bss 영역 확장
.ps2
.open "build/files/file_00000.elf", 0xff000

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
	dd 0x797428 + 0x40000

.close

.ps2
.open "build/files/file_00000.elf", 0x0

.org 0x484d7c
	dd 0x2189a8 + 0x40000

.close
