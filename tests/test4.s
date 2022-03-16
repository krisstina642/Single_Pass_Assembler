.global a
.text
    jeq *a(%pc)
    jeq e
    jeq *d(%pc)
d:  .word d
    mov %r0,e
.data
.skip 8
.equ m, 7 - i + a +d 
.equ i, m
e:  .word 5, a 
a:  .word bss
.bss
  .skip 8
.end