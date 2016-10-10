#!/bin/bash
filelist=`find $1|grep .root|grep .phys.hit.`
echo ${filelist}
for X in ${filelist}; do
	echo "TOF build : ${X}"
	./TOF-build -t root -f ${X}
done
paramlist=""
for X in `find $1|grep .root|grep .TOF.stat.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./TOF-hists-before${paramlist} > ${1}/TOF-cut.txt

filelist=`find $1|grep .root|grep .phys.hit.`
echo ${filelist}
for X in ${filelist}; do
	echo "TOF cut : ${X}"
	./TOF-cut-preselect -t root -f ${X} < ${1}/TOF-cut.txt
done

paramlist=""
for X in `find $1|grep .root|grep .TOF.cut.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./TOF-hists-after${paramlist} > ${1}/TOF-cut.txt
