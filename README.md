Software for J-PET detector time synchronization
================================================
Information about J-PET project
http://koza.if.uj.edu.pl/pet/


Conditions of measurements
==========================

This software can be used for measurements with PET detector that consists of scintillator strips having photomultipliers in their edges.
When two strips register gamma-quanta from one positron anihillation one can determine each gamma-quantum hit position and time from the times of signals from photomultipliers installed at the edges of the scintillator strips.
The hits positions and times allow give enough information for reconstruction of the anihillation point position.

For performing such reconstruction one needs to synchronize all photomultipliers with each other i.e. to obtains the time offsets created by electronics that one needs to substract.
This software calculates the set of offsets one needs to add but they can have any sign.

For the synchronization one needs the data from measurements performed with point-like beta-plus active source placed in the colimator that allows the direct anihillation gamma-quanta to hit only into the middles of the strips.
This condition allows to easily synchronize the signals from both edges for every single strips.

For synchronizing the oposite strips the software analyses the coincidents considering them to be a product of true anihillations.
In such conditions the true hits would occur at the same time if the source is exactly in the center point of the detector.

For synchronizing the neighbour strips the software analyzes the coinsidences corresponding to scattering the quanta by one strip and registering the scattered quanta by another one.


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

Each sub-directory means new configuration type.
The default one is "run1".
Please note that each configuration contains also include file.
It means that changing configuration requires recompilation of the software.
Nevertheless, cmake allows to create several builds for different configurations.
If you want to create new configuration you'd better copy the files from run1 config and then change options you 
are interested in.


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


Input and output
================

The input data format is HLD and the output format for time offsets in plain text.
Lets consider the HLD files from the measurement described above to be stored in /path/to/the/data.
During the analysis the software also generates ROOT files for events data and TXT files with such information as:

- Positions of peaks on t_A-t_B spectra (AB-center.txt)

- Positions of peaks on t_hit1-t_hit2 spectra for true anihillation coincidences (Oposite-strips.txt)

- Positions of peaks on t_hit1-t_hit2 spectra for coincidences conditioned by scattering (Scattered.txt)

- The final result - time offsets for synchronization (Deltas*.new.txt)

All these files are stored in the same directory with data files.


Debug output
============

The software also stores huge amount of debug information in build directory.
If one has gnuplot installed in the system the plots with this information will be stored in PNG files.
When this software is used once more for another data these files are overwritten.

