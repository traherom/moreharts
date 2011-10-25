/*  vulp.c  */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DELAY 3

int main()
{
	char * fn = "XYZ"; // CHANGE TO NOT BE IN /tmp/
	char buffer[60];
	FILE *fp;

	/* get user input */
	scanf("%50s", buffer );
	printf("VULP: Going to write: %s\n", buffer);

	if(!access(fn, W_OK)){
		/* simulating delay */
		sleep(DELAY); // CHANGE FROM LOOP DELAY

		printf("VULP: Opening file...\n");
		fp = fopen(fn, "a+");
		if(fp == NULL)
		{
			perror("VULP");
			printf("VULP: File not opened\n");
			return 1;
		}
		
		printf("VULP: Writing...\n");
		fwrite("\n", sizeof(char), 1, fp);
		fwrite(buffer, sizeof(char), strlen(buffer), fp);
		printf("VULP: Closing...\n");
		fclose(fp);
	}
	else printf("VULP: No permission \n");

	return 0;
}

