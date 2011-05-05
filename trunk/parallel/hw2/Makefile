TASKS := 6

reduction : reduction.o
	gcc -lm reduction.o -o reduction
	
reduction.o : reduction.c
	gcc -c reduction.c
	
run : reduction
	./test.sh ${TASKS}

