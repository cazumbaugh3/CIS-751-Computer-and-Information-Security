# CIS 751 Lab Assignment 1

## Task 1
We can view, set, and unset shell variables as shown in Figure 1. In addition to the `printenv` command, we can also use `grep` as follows:
```sh
env | grep "LOGNAME"
```

![Task 1](./Screenshots/task1.png)

## Task 2

## Task 3
Initially, the program called `execve()` in the following way:
```c
execve("/usr/bin/env", argv, NULL);
```

The `execve` function receives it's environment variables from the caller. As described in the documentation, the signature for `execve` is:
```c
int execve(const char *filename, char *const argv[], char *const envp[]);
```

Where `envp` are environment variables passed by the caller to the new process.

The initial program called
```c
execve("/usr/bin/env", argv, NULL);
```

Which passes `NULL` for the environment variables. As seen in Figure 2, this results in nothing being printed when `/usr/bin/env` is called.

The updated program called
```c
execve("/usr/bin/env", argv, environ);
```

Which passes the environment variables in `environ` (the parent's environment variables) to the new process. We observe that the parent process's environment variables are correctly passed to the child and thus printed when `/usr/bin/env` is called.


![Task 3](./Screenshots/task3.png)

## Task 4
Unlike `execve()`, `system()` inherits it's environment variables from it's parent. When `system()` spawns a new shell to execute the `env` command, we expect `env` to run in an environment that contains all of the parent process's environment variables. As expected, these are printed when the following line is executed:
```c
system("/usr/bin/env");
```

![Task 4](./Screenshots/task4.png)

## Task 5
To set this up, the following shell variables were created (or modified):
```sh
export PATH = my_path:$PATH
export MY_VAR=foo
export LD_LIBRARY_PATH=my_linker_path
```
The program was then run and output was put in `task5_output`. The resulting file was then searched using 
```sh
grep <VARIABLE> task5_output
```
Figure 5 shows the output of this. As is evident, the `MY_VAR` and `PATH` variables were inherited by the child process, and are thus present in the output file. However, the `LD_LIBRARY_PATH` variable is not present. This is a security feature where certain environment variables are not passed to the child if it is created in a Set UID context.

![Task 5a](./Screenshots/task5.png)

We can test this by removing the Set UID bit and re-running the experiment. As is shown in Figure 6, the `LD_LIBRARY_PATH` variable is inherited by the child.

![Task 5b](./Screenshots/task5_b.png)

## Task 6
In this exercise, we will exploit the program calling `ls` and use it to generate a root shell. As described in the lab, we first need to setup the following symbolic link:
```sh
$ sudo rm /bin/sh
$ sudo ln -s /bin/zsh /bin/sh
```

The `PATH` variable is updated to contain the current directory and exported as follows:
```sh
export PATH=/home/seed:$PATH
```

We then compile the following program with the name `ls` in the "/home/seed" directory.
```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("I am a malicious version of ls!!!!\n");
    printf("I will now spawn a root shell!\n");
    system("/bin/sh");
    return 0;
}
```

*Note that the task6 program was given the owner root and the set UID bit.*

As is shown in Figure 7, we get a root shell when the task6 program is run. This occurs because the system will search the directories described in `PATH` to find the `ls` command when run. Since our directory preceeds everything else, it will find a match to our version of `ls` and execute that, spawning a shell in a Set UID context.

![Task 6](./Screenshots/task6.png)

## Task 7
In task 7, we create a malicious library containing a `sleep` function and attempt to get our program to execute the malicious function. We do this by compiling our program (shown below) as a library and adding it to `LD_PRELOAD`, so when the dynamic linker searches for `sleep`, it finds our function first.
```c
// Malicious sleep program
#include <stdio.h>
void sleep(int s)
{
    printf("I am not sleeping!\n");
}
```

We first do this in a non-privileged environment (the task7 program is owned by seed and is not Set UID). As can be seen in Figure 8, when task7 is executed we see the string `"I am not sleeping"` printed, as the malicious function was executed. However, when task7 is owned by root and is a Set UID program (escalated privileges), we do not see this string printed and the program simply slept for a second before exiting. This is because the `LD_PRELOAD` environment variable is not passed to the new shell in a Set UID context. This is a security feature to prevent exactly this.

![Task 7a](./Screenshots/task7_a.png)

Things work a little different when we execute task7 in a root account where the `LD_PRELOAD` variable was exported by root. As shown in Figure 9, our program calls the malicious `sleep()` function demonstrating that the `LD_PRELOAD` environment variable was passed to the child process despite task7 being a root-owned Set UID program.

![Task 7b](./Screenshots/task7_b.png)

Making the program a non-root owned Set UID program and exporing `LD_PRELOAD` in a different, non-root, account yields similar behavior to that shown in Task 7a. Figure 10 shows that the malicious `sleep()` function is not called, demonstrating that the `LD_PRELOAD` environment variable is not passed to the child.

![Task 7c](./Screenshots/task7_c.png)

From these experiments, we can make several conclusions:
1. The `LD_PRELOAD` environmental variable is inherited by the child process in a non-set UID context.
2. The `LD_PRELOAD` environmental variable is generally not inherited by the child process if the program is a Set UID program.
3. Environment variables exported by the root account are always inherited by the child process. This is demonstrated in Figure 9, where the malicious `sleep()` function was called despite the program being Set UID.

## Task 8
### Part 1
Yes, we are still able to compromise the integrity of the system. The simplest way to do this is to exploit the user generated parameter being passed to the `system()` function. For example, we may want to add some additional data to a file that we should have readonly permissions to. Figure 11 demonstrates this by appending text data to a file that we would otherwise not have permission to read or write. By passing the input 

```sh
task8 "testfile.txt; echo "Compromised" >> testfile.txt"
```

we are able to append the string `"Compromised"` to `testfile.txt`, despite not having permissions to read or write to it. Since task8 is a Set UID program, the `system()` command is executed with root privileges. Since root is able to read and write to `testfile.txt`, our command is successful and we can modify the file. In principle, we could create, modify, and delete any file we want through this program.

![Task 8a](./Screenshots/task8_a.png)

### Part 2
Now that we are using `execve()` instead of `system()` to execute the command we are no longer able to exploit the same vulnerability in the program. When we execute the same command we now receive an error:
```
/bin/cat: 'testfile2.txt; echo Compromised >> testfile2.txt": No such file or directory
```

Unlike before, we cannot execute arbitrary commands using command injection. When `system()` is called it effectively executes the following:
```sh
/bin/sh -c "/bin/cat testfile.txt; echo 'Compromised' >> testfile.txt"
```

Thus, a shell is first spawned, and the input is then passed as the command for the shell to execute. When the command is parsed, two parts get executed: 1) "/bin/cat testfile.txt" and 2) echo 'Compromised' >> testfile.txt.

However, `execve()` does not spawn a new shell to execute the command. It simply directly executes the program (i.e. /bin/cat) and passes it the input. Thus, /bin/cat looks for it's input which doesn't exist in this case and we get an error. Since there is no shell involved, the input is not parsed and our command fails.