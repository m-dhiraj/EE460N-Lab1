final: main.o
	gcc -otest main.o 
main: main.c BST312.h kolbe.txt
	gcc -c main.c
