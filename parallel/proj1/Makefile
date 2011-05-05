all : pi_p pi_s pi_simpsons_mpi pi_simpsons_single

pi_p : pi_p.c
	mpicc -o pi_p pi_p.c
	
pi_s : pi_s.c
	gcc -o pi_s -lrt pi_s.c

pi.o : pi.c
	gcc -c pi.c

pi_simpsons_mpi.o : pi_simpsons_mpi.c
	mpicc -c pi_simpsons_mpi.c

pi_simpsons_single.o : pi_simpsons_single.c
	gcc -c pi_simpsons_single.c

pi_simpsons_mpi : pi_simpsons_mpi.o pi.o
	mpicc -o pi_simpsons_mpi -lrt pi_simpsons_mpi.o pi.o

pi_simpsons_single : pi_simpsons_single.o pi.o
	gcc -o pi_simpsons_single -lrt pi_simpsons_single.c pi.c

clean :
	rm pi_p pi_s pi_simpsons_single pi_simpsons_mpi *.o


