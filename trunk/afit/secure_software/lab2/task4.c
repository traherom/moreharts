/*************************************
* Lab 2 - Task 4 - Fix string format vulnerability
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

char prompt(char *pass)
{
	printf("Password: ");
	scanf("%24s", pass);
	fflushstdin();
	
	// Length ok?
	if(strlen(pass) > 20)
		return 0;
	
	// Check each character is in [a-zA-Z0-9]
	char *c = pass;
	while(*c != 0)
	{
		if((*c >= '0' && *c <= '9')
			||
			(*c >= 'A' && *c <= 'Z')
			||
			(*c >= 'a' && *c <= 'z')
			)
		{
			// Valid character, go to the next one
			c++;
		}
		else
		{
			// Invalid
			return 0;
		}
	}
	
	// Passed all the tests, the password is clean
	return 1;
}

int main(int argc, char **argv)
{
	char *pass;
	
	// Allocate a bit of extra space so that we can detect a string that's too long
	pass = calloc(25, sizeof(char));

	// Get password of the right length
	while(!prompt(pass))
	{
		printf("The password must be only alphanumeric and <= 20 characters long\n");
	}
		
	// Fixed vulnerability
	printf("You entered %s\n", pass);
	
	// Correct?
	if(strcmp("bah", pass) == 0)
		printf("Welcome\n");
	else
		printf("Denied\n");

	// Be proper
	free(pass);

	return 0;
}

