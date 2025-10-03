; 고정된 사이즈의 문자열
.macro fixed_stringn, len, s
start:
	.stringn s
	.if (.-start) > len
		.error "String exceeds fixed size!"
	.endif
	.fill len - (.-start), 0
.endmacro


; 고정된 사이즈의 EOS가 포함된 문자열
.macro fixed_string, len, s
start:
	.string s
	.if (.-start) > len
		.error "String exceeds fixed size!"
	.endif
	.fill len - (.-start), 0
.endmacro


; 알베드 문자가 아닌 데이터 정의
.macro albhed_dummy
	.db -1, 0, 0, 0
	.db  0, 0, 0, 0
.endmacro


; 알베드 문자 정의
.macro albhed, primer_a, primer_b, word_albhed, word_jaum, word_moum
	.db primer_a - 1
	.db primer_b - 1
	.stringn word_albhed
	.stringn word_jaum
	.stringn word_moum
.endmacro


.macro naming_menu_item, id, keypad_index
	.db id, 0x20, keypad_index, 0x00
.endmacro
