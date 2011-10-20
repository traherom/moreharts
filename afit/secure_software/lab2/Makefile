all : task1 task2and3 task4 task5 task6

clean : 
	rm *.o
	rm task1 task2and3 task4 task5 task6

task1 : task1.o
	gcc -o task1 task1.o

task2and3 : task2and3.o
	gcc -o task2and3 task2and3.o
	
task4 : task4.o
	gcc -o task4 task4.o

task5 : task5.o
	gcc -o task5 task5.o

task6 : task6.o
	gcc -o task6 task6.o

task1.o : task1.c
	gcc -fno-stack-protector -c task1.c

task2and3.o : task2and3.c
	gcc -c task2and3.c
	
task4.o : task4.c
	gcc -c task4.c

task5.o : task5.c
	gcc -c task5.c

task6.o : task6.c
	gcc -c task6.c
	
