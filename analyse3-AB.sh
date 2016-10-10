#!/bin/bash
filelist=`find $2|grep .root|grep .phys.hit.TOF.cut.`
echo ${filelist}
for X in ${filelist}; do
	echo "Stage AB : ${X}"
	./SyncAB-preselect -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4AB.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./SyncAB-fits $1${paramlist} > ${2}/AB-center.txt
