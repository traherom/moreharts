/*************************************
* Lab 2 - Task 2/3 - Buffer overruns and format strings
* Ryan Morehart
* 
* Note: program is insecure, notably at
*       the scanf
*************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Shamelessly taken from
// http://cboard.cprogramming.com/c-programming/67891-flush-input-buffer.html#post482360
void fflushstdin( void )
{
    int c;
    while( (c = fgetc( stdin )) != EOF && c != '\n' );
}

void prompt(char *enteredPass)
{
	printf("Password: ");
	scanf("%24s", enteredPass);
	fflushstdin();
}

int main(int argc, char **argv)
{
	char *pass;
	
	// Allocate a bit of extra space so that we can detect a string that's too long
	pass = calloc(25, sizeof(char));

	// Get password of the right length
	prompt(pass);
	while(strlen(pass) > 20)
	{
		printf("The password must be 20 or fewer characters\n");
		prompt(pass);
	}
		
	// Make ourselves vulnerable
	printf("You entered ");
	printf(pass); // Task 3 opening
	printf("\n");
	
	// Correct?
	if(strcmp("bah", pass) == 0)
		printf("Welcome\n");
	else
		printf("Denied\n");

	// Be proper
	free(pass);

	return 0;
}

