#!/bin/bash
for X in `find $1|grep .hld`; do
	./Prepare-preselect -t hld -f ${X}
done
