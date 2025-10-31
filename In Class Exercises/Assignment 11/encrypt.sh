#!/bin/bash

for i in {1..13}; do
	openssl rsautl -encrypt -pubin -inkey public_key.pem -in "article_chunk${i}.txt" -out "enc_chunk${i}.bin"
done
