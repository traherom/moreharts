/*************************************
* Lab 1 - Simple Password Prompt
* Ryan Morehart
* 
* Note: program is insecure, notably at
*       the scanf
*************************************/
#include <stdio.h>

int main(int argc, char **argv)
{
	char enteredPass[31];
	char actualPass[] = "bah";

	printf("Password (up to 30 chars): ");
	scanf("%s", enteredPass);

	if(strcmp(actualPass, enteredPass) == 0)
		printf("Match\n");
	else
		printf("DENIED!\n");

	return 0;
}

