#!/bin/bash

oldIFS="$IFS"
IFS=$'\n'

for f in $1/*.pkg ; do
	`dirname $0`/install_pkg.sh $f --batch
done

IFS="$oldIFS"

echo "Press [enter] to close"
read
