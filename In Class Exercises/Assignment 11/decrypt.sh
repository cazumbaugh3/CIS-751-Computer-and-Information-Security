#!/bin/bash

for i in {1..13}; do
	openssl rsautl -decrypt -in "enc_chunk${i}.bin" -inkey private_key.pem -out "chunk${i}_decrypt.txt"
done
