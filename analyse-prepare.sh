#!/bin/bash
for X in `find $1|grep .hld`; do
	echo "Preparing file  ${X}"
	./Prepare-preselect -t hld -f ${X}
done
