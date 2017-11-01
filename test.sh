#!/bin/bash
pass=$1
cores=$2

if [ -z "$pass" ]; then
	echo Usage: $0 passwordToCrack
	exit
fi

make
time ./crack $(echo -n "$pass" | md5sum | cut -f 1 -d ' ') ${#pass} $cores
