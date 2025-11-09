#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
	/* This is a Set-UID program, so EUID is 0.
	 * We will change this before calling fopen()
	 * and update to RUID.
	 */
	uid_t real_uid = getuid();
	uid_t eff_uid = geteuid();
	char *fn = "/tmp/XYZ";
	char buffer[60];
	FILE *fp;

	/* Get user input */
	scanf("%50s", buffer);
	// Drop privilege
	seteuid(real_uid);
	/* fopen() checks EUID, which is now RUID.
	 * Thus, open will fail if the current
	 * user does not have write permission to
	 * the file.
	 */
	if(fp = fopen(fn, "a+")) {
		fwrite("\n", sizeof(char), 1, fp);
		fwrite(buffer, sizeof(char), strlen(buffer), fp);
		fclose(fp);
	} else printf("No permission \n");
	// If needed, can escalate privileges
	// using seteuid(eff_uid)
}
