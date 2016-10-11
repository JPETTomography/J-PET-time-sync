#!/bin/bash
for X in `find $1|grep .hld`; do
	echo "Preparing file  ${X}"
	if [ -e ${X%.*}.phys.hit.root ]; then
		echo "has been analysed"
	else
		echo "starting analysis"
		./Preselect-Signals -t hld -f ${X}
	fi
done
