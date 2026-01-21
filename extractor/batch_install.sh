#!/bin/bash

for f in $1/*.pkg ; do
	`dirname $0`/install_pkg.sh $f --batch
done

echo "Press [enter] to close"
read
