#!/bin/bash
for X in `find $1|grep .hld`; do
	echo "Preparing file  ${X}"
	if [ -e ${X%.*}.phys.hit.means.root ]; then
		echo "has been analysed"
	else
		echo "starting analysis"
		./Prepare-preselect -t hld -f ${X}
	fi
done
