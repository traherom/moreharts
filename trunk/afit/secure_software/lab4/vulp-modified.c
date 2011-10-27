/*  vulp.c  */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DELAY 3

int main()
{
	char * fn = "XYZ"; // CHANGE TO NOT BE IN /tmp/
	char buffer[60];
	FILE *fp;
	struct stat statSave[3];

	printf("VULP: Modified version\n");

	/* get user input */
	scanf("%50s", buffer );
	printf("VULP: Going to write: %s\n", buffer);

	// Save stat value for checking again in a second
	if(stat(fn, &statSave[0]))
	{
		perror("VULP");
		printf("VULP: Unable to verify permissions\n");
		return 1;
	}
		
	if(!access(fn, W_OK)){
		/* simulating delay */
		sleep(DELAY); // CHANGE FROM LOOP DELAY

		// Second stat save
		if(stat(fn, &statSave[1]))
		{
			perror("VULP");
			printf("VULP: Unable to verify permissions\n");
			return 1;
		}

		/* open */
		printf("VULP: Opening file...\n");
		fp = fopen(fn, "a+");
		
		// Check access again, just to make sure it's still good
		if(access(fn, W_OK))
		{
			printf("VULP: Permissions lost\n");
			fclose(fp);
			return 1;
		}
		
		// Final save
		if(stat(fn, &statSave[2]))
		{
			perror("VULP");
			printf("VULP: Unable to verify permissions\n");
			fclose(fp);
			return 1;
		}
		
		// Compare the stat for the file now vs the file we checked the first time
		if(statSave[0].st_ino != statSave[1].st_ino || statSave[1].st_ino != statSave[2].st_ino)
		{
			printf("VULP: File has changed on disk\n");
			fclose(fp);
			return 1;
		}
		
		// Ensure file really opened. We do this after the access()
		// check to minimize the number of statements between the two
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
	else
	{
		printf("VULP: No permission \n");
		return 1;
	}

	return 0;
}

