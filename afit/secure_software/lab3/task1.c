/********
* HW: Secure Sofware Lab 3, task 1
* Author: Ryan Morehart
* 
* Displays all alphabetic characters in a file
********/
#include <stdio.h>
#include <ctype.h>

int main(int argc, char **argv)
{
	char *fileName = 0;
	FILE *file = 0;
	char c = 0;
	char isEmpty = 1;
	char isBinary = 0;
	char containsAlpha = 0;
	
	// Handle command line
	if(argc != 2)
	{
		printf("Usage: %s <file>\n", argv[0]);
		return 0;
	}
	
	fileName = argv[1];

	// Open file
	file = fopen(fileName, "r");
	if(!file)
	{
		printf("Unable to open '%s'\n", fileName);
		return 1;
	}
	
	printf("########## BEGIN ##########\n");
	
	// Read and display
	isEmpty = 1;
	containsAlpha = 0;
	isBinary = 0;
	
	c = fgetc(file);
	while(c != EOF)
	{
		// Well clearly not empty if we got here
		isEmpty = 0;
		
		// Skip non-alphabetic characters
		if(isalpha(c))
		{
			containsAlpha = 1;
			printf("%c", c);
		}
		
		// "Binary?" Is it some sort of control, non-texty character?
		if(c != EOF && (c <= 8 || (c >= 14 && c <= 31) || (c >= 127)))
			isBinary = 1;
		
		// Next!
		c = fgetc(file);
	}

	// Done
	fclose(file);
	if(containsAlpha)
		printf("\n");
	
	printf("########### END ###########\n");
	
	// Any additional info to output?
	if(isEmpty)
		printf("File is empty\n");
	else if(isBinary)
		printf("File is binary\n");
	else if(!containsAlpha)
		printf("File contains no alphabetic characters\n");
	
	return 0;
}

