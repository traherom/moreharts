all : vulp

test : vulp
	bash exploit.sh

clean :
	sudo rm -f vulp vulp.o

vulp : vulp.o
	gcc -o vulp vulp.o
	sudo chown root vulp
	sudo chmod u+s vulp
	
vulp.o : vulp.c
	gcc -Wall -g -c vulp.c
	
