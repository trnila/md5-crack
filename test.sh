#!/bin/sh
pass=$1

if [ -z "$pass" ]; then
	echo Usage: $0 passwordToCrack
	exit
fi

make && time ./crack $(echo -n "$pass" | md5sum | cut -f 1 -d ' ') ${#pass}
