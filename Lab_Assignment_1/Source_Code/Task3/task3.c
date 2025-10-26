#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int main()
{
	char *argv[2];
	
	argv[0] = "/usr/bin/env";
	argv[1] = NULL;

	// execve("/usr/bin/env", argv, NULL); // Task3_1
	execve("/usr/bin/env", argv, environ); // Task3_2

	return 0;
}
