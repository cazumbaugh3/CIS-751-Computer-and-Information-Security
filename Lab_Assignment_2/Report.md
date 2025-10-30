# CIS 751 Lab 2 Report
### Author: Chuck Zumbaugh
### Collaborators: None

## Task 1
In this task we check to see if the `/bin/bash_shellshock` and `/bin/bash` shells are vulnerable to shellshock. Shellshock was a vulnerability in bash that exploited how environment variables are passed to child processes. Specifically, if a bash function is written as a string and saved as an environment variable (ex. `foo='() { echo Hello, World; };'`) it will be parsed as a function when the environment variables are inherited by a child process. Moreover, bash parses and executes when evaluating this, so subsequent commands will be executed when the function is parsed. For example, the string `"Executed"` will be printed when the below environment variable is inherited by a child process.
```sh
FOO='() { echo junk; }; echo Executed'
``` 

This behavior is demonstrated in Figure 1. If we create the following shell variable:
```sh
export FOO='() { echo Hello; }; echo vulnerable'
```
We would expect a vulnerable version of bash to print the string `"vulnerable"`. As a result, we can execute arbitrary commands using the above method.

![Task 1: /bin/bash_shellshock shell](./Screenshots/task1_a.png)

However, Figure 2 illustrates that the updated version of bash is not susceptible to this vulnerability. When the new bash shell is spawned, `"vulnerable"` is not printed. Upon further inspection, it is evident that the environment variable `FOO` has not been parsed as a function and remains a string.

![Task 1: /bin/bash shell](./Screenshots/task1_b.png)

## Task 2
Figure 3 illustrates how we can use a simple CGI script to return some data on a GET request. This program simply prints out `"Hello World"` when executed. 
```sh
#!/bin/bash_shellshock

echo "Content-type: text/plain"
echo
echo
echo "Hello World"
```

Ultimately, this is the script we will exploit and is can be called using the following.
```sh
curl http://localhost/cgi-bin/myprog.cgi
```