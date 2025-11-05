.loadtable "tables/armips.tbl"

.include "asm/defines.asm"
.include "asm/macros.asm"


.ps2
.open "build/files/file_00455.ftcx", 0x798000 - 0x40

.orga 0x40
inject_begin:
	.include "asm/variables.inject.asm"
	.include "asm/battle.inject.asm"
	.include "asm/font.inject.asm"
	.include "asm/albhed.inject.asm"
	.include "texts/battle3/mon_names.asm"
	.include "texts/menu/albhed_to_ko_table.asm"
inject_end:

.orga 0x24
	dw inject_end - inject_begin + 0x40
.orga 0x30
	dw font_width_table - inject_begin + 0x40

.close


.ps2
.open "build/files/file_00000.elf", 0xff000
	.include "asm/memory.asm"
	.include "asm/region.asm"
	.include "asm/font.asm"
	.include "asm/albhed.asm"
	.include "asm/battle.asm"
	.include "asm/naming.asm"
	.include "asm/sphere_grid.asm"
.close
