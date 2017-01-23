client: client.o helper.o
	gcc -o client client.o helper.o -Wall

client.o: client.c helper.h
	gcc -o client.o client.c -c -ansi -pedantic -Wall

helper.o: helper.c helper.h
	gcc -o helper.o helper.c -c -ansi -pedantic -Wall
