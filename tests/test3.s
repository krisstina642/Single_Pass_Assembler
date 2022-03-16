.global a,b
.extern c

.data
.word a,2, 3
.equ d, -e + b + 2 + a
.byte 5,6

.text
	mov  0, %r0
a:	mov %r1, b
e:	call c
	addb 3, %r0
	push (%r4)
b:	popb %r4
	cmp  0, %r0
	jeq e
	jeq *e(%pc)
.end
	