Software for J-PET detector time synchronization
================================================
Information about J-PET project
http://koza.if.uj.edu.pl/pet/


Conditions of measurements
==========================

This software can be used for measurements with PET detector that consists of scintillator strips having photomultipliers in their edges.
When two strips register gamma-quanta from one positron anihillation one can determine  the anihillation point position.
This reconstruction requires times of signals from four photomultipliers connected to the two strips that registered anihillation gamma-quanta.

For performing such reconstruction one needs to synchronize all photomultipliers with each other i.e. to obtain the time offsets created by electronics that one needs to substract.
This software calculates the set of offsets one needs to apply for permorming time synchronization.

For the synchronization one needs the data from measurements performed with point-like beta-plus radioactive source placed in the center point with colimator that allows the anihillation gamma-quanta to hit only into the middles of the strips.
This condition allows to easily synchronize the signals from both edges for every single strips.

For synchronization of strips with each other the software analyses coincidences of anihillation quanta and ones caused by scattering.

How to compile the software
===========================

	git clone https://github.com/alexkernphysiker/J-PET-time-sync.git
	
	cd J-PET-time-sync
	
	git submodule update --init --recursive
	
	cd ..
	
	mkdir J-PET-time-sync-build
	
	cd J-PET-time-sync-build
	
	cmake -Dconfig_type=___ ../J-PET-time-sync
	
	make

cmake variable config_type means J-PET configuration needed for time synchronization.
View available configurations in the directory

	configs/

and write one of them instead of ___ in this listing.
Each sub-directory of config/ directory means new configuration type.
The default one is "run1".
Please note that each configuration contains also include file.
It means that changing configuration requires recompilation of the software.
Nevertheless, cmake allows to create several builds for different configurations.
If you want to create new configuration you'd better copy the files from run1 config and then change options you 
are interested in.
Only file j-pet-setup.json needs to be created from scratch (see manual from petwiki).


How to use the software
=======================

The commands needed to perform the analysis look so:

	cd J-PET-time-sync-build

	./analyse1-prepare.sh /path/to/the/data

	./analyse2-sync.sh A /path/to/the/data

First script starts reconstructing hits from RAW data.
The second script runs analysis for reconstructed hits.
The parameter 'A' means using first threshold. B, C and D mean respectively other thresholds.
ATTENTION: it's important to use capital letters.
If one uses more than one letter such as 'AB' it means that average will be taken as signal time.

If you have just performed an analysis for one threshold and want to start analysis for different threshold please run

	rm -f /path/to/the/data/*.Stat4*.root


to clean data after previous analysis.

Input and output
================

The input data format is HLD and the output format for time offsets in plain text.
Lets consider the HLD files from the measurement described above to be stored in /path/to/the/data.
During the analysis the software also generates ROOT files for events data and TXT files with such information as:

- Positions of peaks on t_A-t_B spectra (AB-center.txt)

- Positions of peaks on t_hit1-t_hit2 spectra for true anihillation coincidences (Oposite-strips.txt)

- Positions of peaks on t_hit1-t_hit2 spectra for coincidences conditioned by scattering (Scattered.txt)

- The final result - time offsets for synchronization (Deltas*.txt)

All these files are stored in the same directory with data files.


Debug output
============

The software also stores huge amount of debug information in build directory.
If one has gnuplot installed in the system the plots with this information will be stored in PNG files.
When this software is used once more for another data these files are overwritten.

Generating report text file
===========================

When you performed analysis for all four thresholds you can generate the text file in the report format.
There's a tool with the binary ./report that is responsible for this function.
If you have four text files: DeltaA.txt ... DeltaD.txt you can execute the command

	(cat DeltaA.txt;cat DeltaB.txt;cat DeltaC.txt;cat DeltaD.txt)|./report out > report.txt

and this will generate one text file with needed format.

If you have this report.txt file and you need to have one of DeltaX.txt files to use them for applying time deltas in your analysis with respective j-pet-framework class you may execute the following commands

	cat report.txt|./report in 1 > DeltaA.txt

	cat report.txt|./report in 2 > DeltaB.txt

	cat report.txt|./report in 3 > DeltaC.txt

	cat report.txt|./report in 4 > DeltaD.txt

And you will get back the files that were initially generated by software from this repository
