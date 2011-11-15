all : task3 readme

clean :
	rm -f task3 *.o *.aux *.log *.pdf

test : task3
	./task3

readme : readme.tex
	pdflatex readme.tex	

task3 : task3.o
	gcc -o task3 task3.o
	
task3.o : task3.c
	gcc -Wall -c task3.c
	
