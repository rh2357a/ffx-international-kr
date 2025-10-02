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

