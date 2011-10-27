all : vulp vulp-mod

test : vulp
	bash exploit.sh vulp
	
test-mod : vulp-mod
	bash exploit.sh vulp-mod

clean :
	sudo rm -f vulp vulp-mod *.o critical_script XYZ

vulp-mod : vulp-modified.o
	gcc -o vulp-mod vulp-modified.o
	sudo chown root vulp-mod
	sudo chmod u+s vulp-mod

vulp : vulp.o
	gcc -o vulp vulp.o
	sudo chown root vulp
	sudo chmod u+s vulp
	
vulp-modified.o : vulp-modified.c
	gcc -Wall -g -c vulp-modified.c
	
vulp.o : vulp.c
	gcc -Wall -g -c vulp.c
	
