#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *devrand = NULL;
	int i = 0;
	unsigned int rand = 0;
	
	// Open /dev/random
	devrand = fopen("/dev/random", "rb");
	if(devrand == NULL)
	{
		printf("Unable to open /dev/random\n");
		return 1;
	}
	
	// Plop out 10 random numbers from it
	for(i = 0; i < 10; i++)
	{
		// Fill the random variable with random bits
		fread(&rand, sizeof(int), 1, devrand);
		printf("%u\n", rand);
	}
	
	// Done
	fclose(devrand);
	return 0;
}

