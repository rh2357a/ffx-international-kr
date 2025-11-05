; 한글 이름 표시 (버그 수정)
.if MULTILANG == 0
.org 0x23e6bc
	b @_lab_0023e6e0
.org 0x23e6e0
@_lab_0023e6e0:
	; ...
.endif
