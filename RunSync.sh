#!/bin/bash
./analyse1-prepare.sh $2
./analyse2-AB.sh $1 $2
./analyse3-strips.sh $1 $2
