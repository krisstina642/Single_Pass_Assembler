.global a
.section sek1 rxp
    jeq *a(%pc)
    jeq e
    jeq *d(%pc)
d:  .word d
    mov %r0,e
.section sek2 rwp
.skip 8
.equ m, 7 - i + a +d 
.equ i, d
e:  .word 5, a 
a:  .word sek3
.section sek3
  .skip 8
.end

