#!/bin/bash
for X in `find $2|grep .root|grep .Hits.`; do
	echo "Stage AB : ${X}"
	if [ -e ${X%.*.*}.Stat4AB.root ]; then
		echo "has been analysed"
	else
		echo "starting analysis"
		(cat ${2}/Deltas.txt)|./Preselect-AB -t root -f ${X}
	fi
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4AB.`; do
	paramlist=${paramlist}" "${X}
done
./TOT-hists -coincidence${paramlist} > /dev/null
./Fit-AB $1${paramlist} > ${2}/AB-center.txt
