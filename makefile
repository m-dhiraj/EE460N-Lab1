inal: main.o
	gcc  -o assemble main.o 
main: main.c  kolbe.txt
	gcc -c main.c