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
	li v1, 20
	mult a0, a0, v1
	addu v0, v0, a0

@@_return:
	ld ra, 0x0(sp)
	jr ra
	addiu sp, sp, 0x10

