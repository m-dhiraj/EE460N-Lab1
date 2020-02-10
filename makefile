inal: main.o
	gcc  -otest main.o 
main: main.c  kolbe.txt
	gcc -c main.c