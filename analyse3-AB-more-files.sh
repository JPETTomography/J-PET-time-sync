#!/bin/bash
filelist=`find $2|grep .root|grep .Hits.`
echo ${filelist}
for X in ${filelist}; do
	echo "Stage AB : ${X}"
	if [ -e ${X%.*.*}.Stat4AB.root ]; then
		echo "has been analysed"
	else
		echo "starting analysis"
		./Preselect-AB -t root -f ${X}
	fi
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4AB.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./Fit-AB $1${paramlist} > ${2}/AB-center.txt
