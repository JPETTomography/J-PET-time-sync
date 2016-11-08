#!/bin/bash
for X in `find $2|grep .root|grep .Hits.`; do
	echo "Stage Strips : ${X}"
	(cat ${2}/Deltas.txt  & cat ${2}/TOT-cut.txt & cat ${2}/AB-center.txt)|./Preselect-StripsSync -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4Strips.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./Fit-oposite $1${paramlist} > ${2}/Oposite-strips.txt
./Fit-neighbour $1${paramlist} > ${2}/Neighbour-strips.txt

(cat ${2}/Deltas.txt)|./Solve-equations $1 ${2}/AB-center.txt ${2}/Oposite-strips.txt ${2}/Neighbour-strips.txt > ${2}/Deltas.new.txt
