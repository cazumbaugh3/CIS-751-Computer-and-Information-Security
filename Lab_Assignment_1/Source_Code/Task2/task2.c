#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

extern char **environ;
/* printenv() prints all environment variables of the
 * current process.
*/
void printenv()
{
	int i = 0;
	while (environ[i] != NULL) {
		printf("%s\n", environ[i]);
		i++;
	}
}

void main()
{
	pid_t childPid;

	switch(childPid = fork()) {
		case 0: /* Child process */
			//printenv(); // Commented for b.out
			exit(0);
		default: /* Parent process */
			printenv(); // Commented for a.out
			exit(0);
	}
}




























































































