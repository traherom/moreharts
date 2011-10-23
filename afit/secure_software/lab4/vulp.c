/*  vulp.c  */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DELAY 2

int main()
{
	char * fn = "/tmp/XYZ";
	char buffer[60];
	FILE *fp;
	long int  i;

	/* get user input */
	scanf("%50s", buffer );
	printf("Going to write: %s\n", buffer);

	if(!access(fn, W_OK)){
		/* simulating delay */
		sleep(DELAY);

		printf("Opening file...\n");
		fp = fopen(fn, "a+");
		printf("Writing...\n");
		fwrite("\n", sizeof(char), 1, fp);
		fwrite(buffer, sizeof(char), strlen(buffer), fp);
		printf("Closing...\n");
		fclose(fp);
	}
	else printf("No permission \n");

	return 0;
}

