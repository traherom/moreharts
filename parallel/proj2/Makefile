PROCS := 1
MAX := 30
MODE := 0

all: framework

framework: mod0.o mod1.o mod2.o util.o framework.o
	mpicc -o framework framework.o util.o mod0.o mod1.o mod2.o

framework.o: framework.c mod0.h mod1.h mod2.h util.h
	mpicc -c framework.c
	
mod0.o: mod0.c mod0.h util.h
	mpicc -c mod0.c
	
mod1.o: mod1.c mod1.h util.h
	mpicc -c mod1.c
	
mod2.o: mod2.c mod2.h util.h
	mpicc -c mod2.c

util.o: util.c util.h
	mpicc -c util.c

clean:
	rm *.o framework

run: all
	mpirun -np ${PROCS} -hostfile lamHosts ./framework ${MODE} ${MAX}

run_csv: all
	@mpirun -np ${PROCS} -hostfile lamHosts ./framework -t ${MODE} ${MAX}

test: all
	bash test.sh

