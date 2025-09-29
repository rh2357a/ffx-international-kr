.macro fixed_stringn, len, s
start:
	.stringn s
	.if (.-start) > len
		.error "String exceeds fixed size!"
	.endif
	.fill len - (.-start), 0
.endmacro
