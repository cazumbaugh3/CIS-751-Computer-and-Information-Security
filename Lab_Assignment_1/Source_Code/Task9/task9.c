#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void main()
{
	int fd;

	/* Assume that /etc/zzz is an important system file.
	 * and it is owned by root with permission 0644.
	 */
	fd = open("/etc/zzz", O_RDWR | O_APPEND);
	if (fd == -1) {
		printf("Cannot open /etc/zzz\n");
		exit(0);
	}

	// Simulate tasks conducted by the program
	sleep(1);

	/* After the task, the root privileges are no longer
	 * needed, it's time to relinquish root privileges
	 * permanantly. 
	 */
	setuid(getuid());

	if (fork()) {
		close(fd);
		exit(0);
	} else {
		/* Now assume the child process is compromised,
		 * malicious attackers have injected the 
		 * following statements into the process
		 */
		write(fd, "Malicious Data\n", 15);
		close(fd);
	}
}
