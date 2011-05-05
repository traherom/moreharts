POCS := 1
BOARD := boards/board.txt
TIME := 5
DISPLAY := 1

all: main.o util.o
	mpicc -Wall -o life main.o util.o

main.o: main.c util.h
	mpicc -Wall -c main.c

util.o: util.c util.h
	mpicc -Wall -c util.c

clean:
	rm *.o life

run: all
	mpirun -np ${PROCS} -hostfile lamHosts ./life ${BOARD} ${TIME} ${DISPLAY}

