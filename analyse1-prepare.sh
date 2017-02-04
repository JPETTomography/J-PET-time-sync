#!/bin/bash
for X in `find $1|grep .hld|grep -v .root`; do
	echo "File  ${X}"
	if [ -e ${X%.*}.Hits.root ]; then
		echo "has been analysed"
	else
	    if [ -e ${X}.tmp ]; then
		echo "is being analysed"
	    else
		touch ${X}.tmp
		echo "starting analysis"
		./Preselect-Hits -t hld -f ${X}
		rm ${X}.times.root
		rm ${X}.raw.root
		rm ${X}.root
		rm ${X}.tmp
	    fi
	fi
done
