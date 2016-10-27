#!/bin/bash
paramlist=""
for X in `find $1|grep .root|grep .Signals.`; do
	paramlist=${paramlist}" "${X}
done
./TOT-hists -before-cut${paramlist} > ${1}/TOT-cut.txt

for X in `find $1|grep .root|grep .Signals.`; do
	echo "TOT cut : ${X}"
	(cat ${1}/TOT-cut.txt)|./Preselect-Hits -t root -f ${X}
done

paramlist=""
for X in `find $1|grep .root|grep .Hits.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./TOT-hists -after-cut${paramlist} > /dev/null
