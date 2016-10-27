#!/bin/bash
for X in `find $2|grep .root|grep .Hits.`; do
	echo "Stage AB : ${X}"
	(cat ${2}/Deltas.txt)| ./Preselect-AB -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4AB.`; do
	paramlist=${paramlist}" "${X}
done
./Fit-AB $1${paramlist} > ${2}/AB-center.txt
