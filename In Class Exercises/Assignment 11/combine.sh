#!/bin/bash

for i in {1..13}; do
	cat "chunk${i}_decrypt.txt" >> decrypted_article.txt
done
