all: batalla clean

batalla: main.o batalla.o batalla.h
	gcc -std=c99 -pthread -o batalla main.o batalla.o

main.o: main.c batalla.h
	gcc -Wall -Wextra -c main.c

batalla.o: batalla.c batalla.h
	gcc -Wall -Wextra -c batalla.c

debug: main.c batalla.c
	gcc -g -c main.c
	gcc -g -c batalla.c
	gcc -o batalla main.o

clean:
	rm -f *.o
