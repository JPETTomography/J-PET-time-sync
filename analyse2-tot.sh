#!/bin/bash
paramlist=""
for X in `find $1|grep .root|grep .Signals.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./TOT-hists-before${paramlist} > ${1}/TOF-cut.txt

filelist=`find $1|grep .root|grep .Signals.`
echo ${filelist}
for X in ${filelist}; do
	echo "TOF cut : ${X}"
	./Preselect-Hits -t root -f ${X} < ${1}/TOF-cut.txt
done

paramlist=""
for X in `find $1|grep .root|grep .Hits.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./TOT-hists-after${paramlist}
