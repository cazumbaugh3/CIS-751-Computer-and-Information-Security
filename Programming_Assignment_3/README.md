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
strcat(matching_pattern, ":"); // strlen(name) + 1 bytes used
strcat(matching_pattern, ssn); // We can cause an overflow here by passing > 16 bytes
```

Clearly, there is a vulnerability in that the buffer size is determined solely by name and there is no check that `ssn` will fit in the remaining 16 bytes. After constructing `matching_pattern`, the score file is scanned anc checked against the input as follows:
```
// line is mutated with each iteration
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

Clearly we can exploit this using an unlink based attack, but we need to ensure the payload is not modified by the program prior to `free()` being called. When there are no matches in the file, which we can safely assume if we are passing an exploit, the `matching_pattern` and `score` data are not changed. However, `line` is mutated during the `while` loop above so the entire payload must be contained within `matching_pattern` and `score`. This is not a problem since we can make `matching_payload` whatever size we need through the `name` argument and use `score` to setup a fake heap structure. The overall strategy will be to use the `name` input to contain the NOP sled and shellcode and use the `SSN` input to fill the remaining portion of the buffer, and overwrite the `score` buffer to create a fake heap structure that will cause an unlink to occur when `free(matching_pattern)` is called.

## Finding the relevant addresses
### Address of `free`
Since `malloc` and `free` are part of the C standard library, the assembly instructions will contain stubs to these functions that will be resolved at runtime by the dynamic linker. Specifically, this stub is the address of the entry in the PLT, which will either return the address from the GOT if it has been previously used, or call the resolver to retrieve the address. For this exploit, we are interested in modifying the address of `free` in the GOT table to point to our payload. This location of GOT entries can be retrieved using
```
$ objdump -R getscore_heap
``` 
Which shows the following for `free`:
```
...
08049d30 R_386_JUMP_SLOT free
```

Thus, the address we need to use in the forward pointer is `0x08048D24`.

### Address of `matching_pattern`
The `getscore_heap` program will print out the address of `matching_pattern`, `score`, and `line` when run, so we can get the address of `matching_pattern` by simply running it with junk input:
```
./getscore_heap aaa aaa

Address of matching_pattern : 0x8049ec8
```

Most programs won't do this though, and in such cases we can use GDB to find the address of variables. To do this, we can set a breakpoint at some point in the program after `matching_pattern` has been allocated. By running `x &matching_pattern`, we will get the location on the stack that contains the address of `matching_pattern`.

## Determining the length of the buffer
Since we have access to the source code, it is quite trivial to determine the length of the buffer. The program allocates `strlen(name) + 17` bytes, so we need to know the length of the input. As previously mentioned, the `name` argument must contain the exploit prefix, the `jmp 0x6` instruction, the overwrite (what will be replaced by the GOT address), and the shellcode. The below table contains the size of each component.

|  Component      | Size (bytes)       |
| :-------------: | :----------------: |
| Prefix          | 8                  |
| NOP JMP         | 8                  |
| Overwrite       | 4                  |
| Shellcode       | 46                 |
| **Total**       | **66**               |
