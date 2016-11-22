#!/bin/bash
paramlist=""
for X in `find $2|grep .root|grep .Hits.`; do
	paramlist=${paramlist}" "${X}
done
echo "Building TOT hists"
./TOT-hists -hits${paramlist} > /dev/null

if [ -e $2/Deltas.txt ]; then
    echo "Deltas.txt exists"
else
    echo "Creating Deltas.txt"
    ./gen_empty > $2/Deltas.txt
fi
for X in `find $2|grep .root|grep .Hits.`; do
	echo "Stage AB : ${X}"
	(cat $2/Deltas.txt)| ./Preselect-AB -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4AB.`; do
	paramlist=${paramlist}" "${X}
done
echo "Building TOT hists"
./TOT-hists -coincidence${paramlist} > /dev/null
echo "Fitting A-B hists"
./Fit-AB $1${paramlist} > $2/AB-center.txt

for X in `find $2|grep .root|grep .Hits.`; do
	echo "Stage Strips : ${X}"
	(cat $2/Deltas.txt & cat $2/AB-center.txt)|./Preselect-StripsSync -t root -f ${X}
done
paramlist=""
for X in `find $2|grep .root|grep .Stat4Strips.`; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./Fit-oposite $1${paramlist} > $2/Oposite-strips.txt
./Fit-neighbour $1${paramlist} > $2/Neighbour-strips.txt

(cat $2/Deltas.txt)|./Solve-equations $1 $2/AB-center.txt $2/Oposite-strips.txt $2/Neighbour-strips.txt > $2/Deltas.new.txt
echo "replacing deltas"
rm $2/Deltas.txt
mv $2/Deltas.new.txt $2/Deltas.txt
