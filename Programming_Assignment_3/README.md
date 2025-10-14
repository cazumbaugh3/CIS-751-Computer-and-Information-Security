# Assignment 3 Documentation
### Written By: Chuck Zumbaugh
### Collaborators: None

## Running the exploit
There are two files included, a `build_exploit.c` file that contains the source code and a compiled binary, `build_exploit`. When run, the compiled binary will generate the two inputs required by `getscore_heap` and spawn a new shell that contains the shell variables `NAME` and `SSN`. To run the exploit, simply build the shell variables and pass them to the program as follows:
```
$ ./build_exploit                   # Builds the shell variables NAME and SSN
$ ./getscore_heap $NAME $SSN        # Run the program with the generated inputs
```

The exploit builder can be compiled from the source if needed using GCC:
```
gcc -o build_exploit build_exploit.c
```

## Overview and objective
The objective is to exploit a buffer overflow vulnerability in the `getscore_heap` program and get the program to return a shell. This program accepts two pieces of user input, a name and a SSN, that are then used to look up data in a `score.txt` file and return the matching data. To do this, it allocates 3 character buffers on the heap: `matching_pattern`, `score`, and `line` (in that order). While the size of `score` and `line` are fixed (10 and 120 bytes, respectively), the size of `matching_pattern` is the length of the `name` argument + 17. The `matching_pattern` is used to store the text pattern to search for, and is built as follows:
```
...

// The length of the buffer is determined by the name argument
if ((matching_pattern = (char *) malloc(strlen(name) + 17)) == NULL) {
    printf("Failed to allocate memory.\n");
    exit(-1);
}

...

strcpy(matching_pattern, name); 
strcat(matching_pattern, ":");
strcat(matching_pattern, ssn); // We can cause an overflow here by passing > 16 bytes
```

Clearly, there is a vulnerability in that the buffer size is determined solely by name and there is no check that `ssn` will fit in the remaining 16 bytes. After constructing `matching_pattern`, the score file is scanned anc checked against the input as follows:
```
while (fgets(line, 120, scorefile) != NULL) {
    if (match_point = str_prefix(matching_pattern, line)) {
        ... // Code not reached assuming str_prefix returns NULL
    }
}
```

After reading through the file the program reaches the end where these variables are freed as follows:
```
free(matching_pattern);
free(score);
free(line);
```

Clearly we can exploit this using an unlink based attack, but we need to be careful with how this is structured. When there are no matches in the file, which we can safely assume if we are passing an exploit, the `matching_pattern` and `score` data is not changed, but `line` is mutated during the `while` loop above.