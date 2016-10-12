#!/bin/bash

filelist=`find $1|grep .root|grep .Signals.`
echo ${filelist}
for X in ${filelist}; do
	echo "File  ${X} for TOT cut"
	if [ -e ${X%.*.*}.Hits.root ]; then
		echo "has been analysed"
	else
		echo "starting analysis"
		./Preselect-Hits -t root -f ${X} < ${1}/TOF-cut.txt
	fi
done

paramlist=""
for X in `find $1|grep .root|grep .Hits.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./TOT-hists-after${paramlist}
