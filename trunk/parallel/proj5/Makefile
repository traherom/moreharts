THREADS := 
MAX := 30

CC := gcc
CC_OPTS := -Wall -fopenmp -lrt -lm

all: threaded

threaded: mod1.o util.o framework.o
	${CC} ${CC_OPTS} -o threaded framework.o util.o mod1.o

framework.o: framework.c mod1.h util.h
	${CC} ${CC_OPTS} -c framework.c
	
mod1.o: mod1.c mod1.h util.h
	${CC} ${CC_OPTS} -c mod1.c

util.o: util.c util.h
	${CC} ${CC_OPTS} -c util.c

clean:
	rm *.o threaded

run: threaded
	./threaded ${THREADS} ${MAX}

test: all
	bash test.sh

