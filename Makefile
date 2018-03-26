CC=gcc
FLAGS=-Wall -Wextra -Wshadow -std=c99 -g
LIBS=-lncurses
BIN=/usr/bin/

csnake: main.c
	${CC} main.c ${FLAGS} ${LIBS} -o csnake

clean:
	rm csnake

