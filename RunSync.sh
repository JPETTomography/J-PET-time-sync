#!/bin/bash
./analyse1-prepare.sh $2
./analyse2-tot.sh $2
./analyse3-AB.sh $1 $2
./analyse4-final.sh $1 $2