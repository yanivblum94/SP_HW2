FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LIBS = -lm

all: main spmat
clean:
	rm -rf *.o spmat main
main: main.o
	gcc main.o - o $(LIBS)
spmat: spmat.o
	gcc spmat.o - o $(LIBS)
main.o: main.c
	gcc $(FLAGS) -c main.c
spmat.o: spmat.c
	gcc $(FLAGS) -c spmat.c



