#!/bin/bash

if [ -e $2/Deltas$1.txt ]; then
    echo "Deltas$1.txt exists"
else
    echo "Creating Deltas$1.txt"
    ./gen_empty > $2/Deltas$1.txt
fi



for X in `find $2|grep .root|grep .Hits.`; do
	echo "Stage AB : ${X}"
	(echo $1;cat $2/Deltas$1.txt)| ./Preselect-AB -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4AB.`; do
	paramlist=${paramlist}" "${X}
done
echo "Fitting A-B hists"
./Fit-AB${paramlist} > $2/AB-center.txt



for X in `find $2|grep .root|grep .Hits.`; do
	echo "Stage Strips : ${X}"
	(echo $1;cat $2/Deltas$1.txt & cat $2/AB-center.txt)|./Preselect-StripsSync -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4Strips.`; do
	paramlist=${paramlist}" "${X}
done
echo "Fitting coincidences spectra"
./Fit-oposite${paramlist} > $2/Oposite-strips.txt
./Fit-scattered${paramlist} > $2/Scattered.txt

(cat $2/Deltas$1.txt)|./Solve-equations $2/AB-center.txt $2/Oposite-strips.txt $2/Scattered.txt > $2/Deltas$1.new.txt
