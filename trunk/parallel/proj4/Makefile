PROCS := 2
DIMENSION := 4
SEED := 0
BLOCKING := 1
CACHE_SIZE := 1024

all: matrix

matrix: main.o util.o
    mpicc -o laird_morehart_matrix main.o util.o

main.o: main.c util.h
    mpicc -c main.c

util.o: util.c util.h
    mpicc -c util.c

clean:
    rm *.o laird_morehart_matrix

run: all
    mpirun -np ${PROCS} -hostfile lamHosts ./laird_morehart_matrix ${DIMENSION} ${SEED} ${BLOCKING} ${CACHE_SIZE} 
