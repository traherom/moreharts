/********
* HW: Secure Sofware Lab 3, task 1
* Author: Ryan Morehart
* 
* cats the given file name, not vulnerable to command injection
********/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_DYN_NAME_LEN 100

// Returns true if the name appears to be "safe". Ha, blacklists
char isFileNameValid(char *name)
{
	char *currChar = 0;
	
	// Check for illegal characters in the file name
	currChar = name;
	while(*currChar != '\0')
	{
		if(*currChar == ';' || *currChar == '&' || *currChar == '|' || *currChar == '`')
			return 0;
		
		currChar++;
	}
	
	// Didn't hit a bad one
	return 1;
}

int main(int argc, char **argv)
{
	char cat[] = "cat";
	char *currFileName = 0;
	int fileNameLength = 0;
	char doneAllocation = 0;
	
	// Check input
	currFileName = argv[1];
	while(!isFileNameValid(currFileName))
	{
		printf("The file name you supplied appears to be invalid.\n");
		printf("Enter a new one (up to 100 characters): ");
		
		// Need to allocate mem?
		if(!doneAllocation)
		{
			currFileName = calloc(MAX_DYN_NAME_LEN + 1, sizeof(char));
			if(!currFileName)
			{
				printf("Out of memory. How sad\n");
				return 1;
			}
			
			doneAllocation = 1;
		}
		
		// Get new input
		fgets(currFileName, MAX_DYN_NAME_LEN, stdin);
		
		// Drop the newline from fgets
		fileNameLength = strlen(currFileName);
		if(currFileName[fileNameLength - 1] == '\n')
			currFileName[fileNameLength - 1] = '\0';
	}
	
	// However, blacklisting is generally bad. Instead, let's
	// use a more secure call that separates commands and parameters
	// This replaces our process. If we had more to do, we'd need to fork before hand
	execlp(cat, cat, currFileName, NULL);
	
	// And yes, we didn't free the currFileName. This would never happen anyway, so why waste the space?
	// if(doneAllocation) free(currFileName);
	return 0;
}

