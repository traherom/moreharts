#include <unistd.h>

int main(int argc, char **argv)
{
	char *screen = "/usr/bin/screen";
	execl(screen, screen, "-R", "-D", NULL);
	return 0;
}

