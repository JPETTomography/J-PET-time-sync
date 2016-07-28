#!/bin/bash
for X in `find $1|grep .hld`; do
	./SyncAB-preselect -t hld -f ${X}
done
paramlist=""
for X in `find $1|grep .root|grep .Stat4AB.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./SyncAB-fits 4${paramlist} > AB-center.txt
for X in `find $1|grep .root|grep .phys.hit.means.`; do
	./SyncStrips-preselect -t hld -f ${X} < AB-center.txt
done
paramlist=""
for X in `find $1|grep .root|grep .Stat4Strips.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
