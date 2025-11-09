#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char *fn = "/tmp/XYZ";
	char buffer[60];
	FILE *fp;

	/* Get user input */
	scanf("%50s", buffer);

	if (!access(fn, W_OK)) {
		fp = fopen(fn, "a+");
		if (fp == NULL) printf("Cannot open file\n"); exit(1);
		fwrite("\n", sizeof(char), 1, fp);
		fwrite(buffer, sizeof(char), strlen(buffer), fp);
		fclose(fp);
	}
	else printf("No permission \n");
}
