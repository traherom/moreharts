/********
* HW: Secure Sofware Lab 3, task 2
* Given in assignment
* 
* Vulnerable to command injection
********/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char cat[] = "cat ";
	char *command = 0;
	size_t commandLength;
	
	commandLength = strlen(cat) + strlen(argv[1]) + 1;
	command = (char *)malloc(commandLength);
	strncpy(command, cat, commandLength);
	strncat(command, argv[1], (commandLength - strlen(cat)));
	
	system(command);
	return 0;
}

