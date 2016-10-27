#!/bin/bash
if [ -e $2/Deltas.txt ]; then
    echo "Deltas.txt exists"
else
    echo "Creating Deltas.txt"
    ./gen_empty > $2/Deltas.txt
fi
                                                                
for X in `find $2|grep .root|grep .Hits.`; do
	echo "Stage AB : ${X}"
	(cat ${2}/Deltas.txt)| ./Preselect-AB -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4AB.`; do
	paramlist=${paramlist}" "${X}
done
./Fit-AB $1${paramlist} > ${2}/AB-center.txt
