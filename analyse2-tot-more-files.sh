#!/bin/bash

for X in `find $1|grep .root|grep .Signals.`; do
	echo "File  ${X} for TOT cut"
	if [ -e ${X%.*.*}.Hits.root ]; then
		echo "has been analysed"
	else
		echo "starting analysis"
		(cat ${1}/TOT-cut.txt)|./Preselect-Hits -t root -f ${X}
	fi
done

paramlist=""
for X in `find $1|grep .root|grep .Hits.`; do
	paramlist=${paramlist}" "${X}
done
./TOT-hists -after-cut${paramlist} > /dev/null
