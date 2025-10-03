; file_00460.bin 구조 (변환 문자, 사전 대응 세트)
; 총 320바이트 (4바이트씩)
; 1: 원본 문자
; 2: 바뀔 문자
; 3: 사전에 해당하는 비트값
; 4: 0


; FUN_002022c0: al bhed 문자 변환
;   a0: str
;   a1: attr
; return
;   0: 해석 필요 없음 (푸르스름한 흰색)
;   1: 미해석 (푸르스름한 흰색)
;   2: 해석 완료 (빨강)
.org 0x2022f4
	la v0, albhed_to_korean
	jr v0
	nop
@_else_other_words:
	lui a0, 0x32
	sb zero, 0x1(s0)
	la v1, 0x3ffffff
	lw v0, -0xfa0(a0)
	xor v0, v0, v1
	beq v0, zero, @@_albhed_master
	li a1, ALBHED_TRANSLATED_COLOR

	li a1, ALBHED_UNTRANSLATED_COLOR

@@_albhed_master:
	b @_return_20240c
	nop


; 색상 지정 방식 수정 (영문 설정)
.org 0x2023d0
	li a1, ALBHED_TRANSLATED_COLOR
	li a1, ALBHED_UNTRANSLATED_COLOR
.org 0x2023ec
	b @_else_other_words
.org 0x20240c
@_return_20240c:
	; ...

; 색상 지정 방식 수정 - 1
.org 0x202d58
	sb v0, 0x33(sp)
	b @_render_font_202d94
	move a0, s1
.org 0x202d94
@_render_font_202d94:
	; ...

; 색상 지정 방식 수정 - 2
.org 0x203670
	sb v0, 0x13(s2)
	b @_render_font_2036b9
	move a0, s1
.org 0x2036b9
@_render_font_2036b9:
	; ...

; 색상 지정 방식 수정 - 3
.org 0x203a50
	sb v0, 0x13(s1)
	b @_render_font_203a98
	move a0, s2
.org 0x203a98
@_render_font_203a98:
	; ...
