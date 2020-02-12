.ORIG	x3000
     lea r1 max
     ldw r1 r1 #0
loop ldb r2 r0 #0
     ldb r3 r0 #1
     stb r2 r0 #1
     stb r3 r0 #0
     add r0 r0 #2
     add r1 r1 #-1
     brp loop
.END
max .FILL	255