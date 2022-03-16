.global a,c
.extern b,abv
.text
    jeq *a(%pc)
    jeq e
    jeq *b(%pc)
    jeq *d(%pc)
d:  .word d
    mov %r0,b
    mov 0xA,b
    mov %r0,e
.data
.skip 8
.equ m, 7 +c
.equ m2, +3 +m +i
.equ i, b
e:  .word 5, 6, l
l:  
    .word c
a:  .word bss
c:  .word b
.bss
  .skip 8
.end
