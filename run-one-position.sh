#!/bin/bash
mylist=`find $1|grep .root|grep .Stat4AB.`
paramlist=""
for X in ${mylist}; do
	paramlist=${paramlist}" "${X}
done
echo ${paramlist}
./SyncAB-fits 4${paramlist}