#!/bin/bash
for X in `find $1|grep .hld|grep -v .root`; do
	echo "File  ${X}"
	if [ -e ${X%.*}.Hits.root ]; then
		echo "has been analysed"
	else
		echo "starting analysis"
 		./Preselect-Hits -t hld -f ${X}
	fi
	rm ${X%.*}.hld.times.root
	rm ${X%.*}.hld.raw.root
	rm ${X%.*}.hld.root
done
