.global a,c
.extern b,abv
.text
    jeq *a(%pc)
    jeq e
    jeq *b(%pc)
    jeq *d(%pc)
d:  .word d
    mov a(%pc),b
    mov l(%pc),b
    mov 0xA,b
    mov %r0,e
.data
.skip 8
.equ m, 7 - m2 + a +d 
.equ m2, i
.equ i, l
e:  .word 5, 6, l
l:  
    .word c
a:  .word bss
c:  .word b
.bss
  .skip 8
.end
