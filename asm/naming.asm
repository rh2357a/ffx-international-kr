; FUN_00234550: 네이밍 엔트리 관련 함수
; FUN_00232df8: 네이밍 메뉴 관련 함수


; 한글 이름 지원
.if MULTILANG == 0

; FUN_00133a88: Rikku name reveal (menu text 0x202c -> name slot 6).
; Korean names are already stored in display encoding; do not pass them
; through FUN_0014d610, whose Japanese-to-Latin table drops Korean glyphs.
.org 0x133aa4
	b 0x133ab8
	; Keep the original delay slot: move a0, s0.

; FUN_0014d718: 이름 복사
.org 0x14d734
	b @_lab_0014d780
	move a0, s0
.org 0x14d780
@_lab_0014d780:
	; ...

; FUN_00233010: 이름 결정
.org 0x233098
	b @_lab_002330cc
	addiu v0, s1, -6
.org 0x2330cc
@_lab_002330cc:
	; ...

.endif

; 가타카나+히라가나 메뉴 데이터
.org 0x327aa0
.area 20, 0
	naming_menu_item 0, 0
	; naming_menu_item 1, 1
	naming_menu_item 2, 2
	naming_menu_item 3, 3
	naming_menu_item 4, 4
.endarea

; Repair legacy empty Rikku names when the name slot is first requested.
; The helper preserves null pointers, other characters and nonempty names.
.org 0x1bddf4
	j repair_empty_rikku_name
	nop
