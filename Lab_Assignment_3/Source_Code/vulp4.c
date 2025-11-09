#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
	char *fn = "/tmp/subdir/XYZ";
	char buffer[60];
	FILE *fp;

	/* Get user input */
	scanf("%50s", buffer);

	if (!access(fn, W_OK)) {
		fp = fopen(fn, "a+");
		fwrite("\n", sizeof(char), 1, fp);
		fwrite(buffer, sizeof(char), strlen(buffer), fp);
		fclose(fp);
	}
	else printf("No permission \n");
}
