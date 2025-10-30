# CIS 751 Programming Assignment 4 (SEH Attack)
### Author: Chuck Zumbaugh
### Collaborators: None

## Exploit
Exploit is due to manipulation of length (`0xFFFF`) with a malicious function name.

## Length of the buffer
Generated 20,000 byte pattern with metasploit. Analysis of EIP during exception handling showed an offset of 16,760 bytes

## Address of POP POP RET
`0x15301506`