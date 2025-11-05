; 창 사이즈 조절
; "메뉴로 돌아가겠습니까?"
.org 0x2653dc
	li a3, 11

; 창 사이즈 조절
; "여기가 괜찮습니까?"
.org 0x265370
	li a3, 9


; 한글 이름 표시 (버그 수정)
.if MULTILANG == 0
.org 0x2601a8
	b @_lab_002601dc
.org 0x2601dc
@_lab_002601dc:
	; ...
.endif
