CC = gcc
CFLAGS = -Wall -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -lm
LDFLAGS =

.PHONY: clean

target: valtlb379.c
	${CC} -o valtlb379 valtlb379.c ${CFLAGS}

clean:
	$(RM) valtlb379 *.o
