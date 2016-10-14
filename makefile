all: batalla clean

batalla: main.o
	gcc -o batalla main.o

main.o: main.c
	gcc -Wall -Wextra -c main.c

debug: main.c
	gcc -g -c main.c
	gcc -o batalla main.o

clean:
	rm -f *.o
