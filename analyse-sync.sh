#!/bin/bash
for X in `find $2|grep .root|grep .phys.hit.means.`; do
	echo "Stage AB : ${X}"
	./SyncAB-preselect -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4AB.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./SyncAB-fits $1${paramlist} > ${2}/AB-center.txt
for X in `find $2|grep .root|grep .phys.hit.means.`; do
	echo "Stage Strips : ${X}"
	./SyncStrips-preselect -t root -f ${X} < ${2}/AB-center.txt
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4Strips.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./SyncStrips-fits${paramlist}
