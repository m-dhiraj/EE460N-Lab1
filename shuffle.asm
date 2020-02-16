.ORIG	x3000
     lea r1 loc
     ldw r1 r1 #0
loop ldb r2 r1 #0
     ldb r3 r1 #1
     stb r2 r1 #1
     stb r3 r1 #0
     add r1 r1 #2
     add r0 r0 #-1
     brp loop
     HALT
loc .FILL	x3500
.END