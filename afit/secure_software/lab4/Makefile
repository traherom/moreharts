all : vulp

test : vulp
	bash exploit.sh

clean :
	sudo rm -f vulp vulp.o

vulp : vulp.o
	gcc -o vulp vulp.o
	chmod u+s vulp
	sudo chown root vulp
	
vulp.o : vulp.c
	gcc -Wall -g -c vulp.c
	
