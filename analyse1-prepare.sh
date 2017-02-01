#!/bin/bash
for X in `find $1|grep .hld|grep -v .root`; do
	echo "File  ${X}"
	if [ -e ${X%.*}.Hits.root ]; then
		echo "has been analysed"
	else
		echo "starting analysis"
 		./Preselect-Hits -t hld -f ${X}
	fi
	rm ${X}.times.root
	rm ${X}.raw.root
	rm ${X}.root
done
