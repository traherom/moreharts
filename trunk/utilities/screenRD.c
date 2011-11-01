/****************
Simple utility to allow you to use
screen as your shell without screwing 
up sftp (basically screen does not handle
-c very well).
****************/
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *screen = "/usr/bin/screen";
	
	// Are we being told to start sftp?
	if(argc == 3 && strncmp(argv[1], "-c", 2) == 0)
	{
		// execute command
		execlp(argv[2], argv[2], NULL);
	}
	else
	{
		// Screen per usual
		execlp(screen, screen, "-R", "-D", NULL);
	}
	
	return 0;
}

