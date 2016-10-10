#!/bin/bash
filelist=`find $2|grep .root|grep .phys.hit.`
echo ${filelist}
for X in ${filelist}; do
	echo "TOF build : ${X}"
	./TOF-build -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .TOF.stat.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./TOF-hists-before $1${paramlist} > ${2}/TOF-cut.txt

filelist=`find $2|grep .root|grep .phys.hit.`
echo ${filelist}
for X in ${filelist}; do
	echo "TOF cut : ${X}"
	./TOF-cut-preselect -t root -f ${X} < ${2}/TOF-cut.txt
done

paramlist=""
for X in `find $2|grep .root|grep .TOF.cut.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./TOF-hists-after $1${paramlist} > ${2}/TOF-cut.txt
