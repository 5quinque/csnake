CC=gcc
FLAGS=-Wall -Wextra -Wshadow -std=c99 -lm -D_POSIX_C_SOURCE=199309L -g
LIBS=-lncurses
BIN=/usr/bin/

csnake: main.c
	${CC} main.c ${FLAGS} ${LIBS} -o csnake

clean:
	rm csnake

