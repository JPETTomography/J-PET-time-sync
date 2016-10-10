#!/bin/bash
for X in `find $1|grep .hld`; do
	echo "Preparing file  ${X}"
	if [ -e ${X%.*}.phys.hit.root ]; then
		echo "has been analysed"
	else
		echo "starting analysis"
		./Prepare-preselect -t hld -f ${X}
	fi
done
filelist=`find $1|grep .root|grep .phys.hit.`
echo ${filelist}
for X in ${filelist}; do
	echo "TOF build : ${X}"
	./TOF-build -t root -f ${X}
done
