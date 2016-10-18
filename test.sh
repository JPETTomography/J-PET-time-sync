#!/bin/bash
filelist=`find $1|grep .root|grep .Hits.`
echo ${filelist}
for X in ${filelist}; do
	echo "Test : ${X}"
	./test1 -t root -f ${X}
	paramlist=""
	for X in `find $1|grep .root|grep .TestStat.`; do
		paramlist=${paramlist}" "${X}
	done
	echo ${paramlist}
	./test2${paramlist}
done
./Fit-AB 1${paramlist}