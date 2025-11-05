; 게임의 원본 구성 설정값
.org 0x569d70
	db 0x00, 0x00, 0x00, 0x00
	; db 0x00, 0x00, 0x00, 0x02


; HDD 설정 비활성화
.org 0x2db190
.area 80, 0
	addiu sp, sp, -0x20
	sd ra, 0x10(sp)

	la a0, 0x5781e8
	jal std_log
	li a1, 0x0

	lui v1, 0x57
	sw zero, -0x7f54(v1) ; 0x5680ac

	ld ra, 0x10(sp)
	jr ra
	addiu sp, sp, 0x20
.endarea


; 국내판 메모리 카드 설정
.org 0x241528
.area 288, 0
	addiu sp, sp, -0x20
	sd ra, 0x10(sp)

	lui a0, 0x33
	lui a1, 0x33
	addiu a1, a1, -0x72b0 ; 0x328d50 = "BKSLPM-67513"
	jal strcpy
	addiu a0, a0, -0x7228 ; 0x328dd8

	lui a0, 0x33
	lui a1, 0x33
	addiu a1, a1, -0x72a0 ; 0x329d60 = "FF0906%02d"
	jal strcat
	addiu a0, a0, -0x7228 ; 0x328dd8

	lui v1, 0x33
	sw zero, -0x7da0(v1) ; 0x328260

	ld ra, 0x10(sp)
	jr ra
	addiu sp, sp, 0x20
.endarea


; 언어 설정 제거
.if MULTILANG == 0
.org 0x235c08
	li v0, 1
	nop
.endif
