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
	
	cmake ../J-PET-time-sync
	
	make

How to use the software
=======================

The input data format is HLD and the output format for time offsets in plain text.
During the analysis the software also generates *.root files for events data and text files with different kind of debug information.
If one has gnuplot installed in the system the plots with debug information will be stored in PNG files.

Lets consider the HLD files from the measurement described above to be stored in /path/to/the/data.
In that case the commands needed to perform the analysis look so:

	./analyse1-prepare /path/to/the/data

	./analyse2-tot /path/to/the/data
	
	./analyse3-AB 1 /path/to/the/data
	
	./analyse4-final 1 /path/to/the/data

The parameter '1' in last two lines means that when the genetic algorithm will run it will be executed in one thread.
One can increase this parameter for parallelizing it.
The deltas will be stored in the same directory with the data in file Deltas.txt.
ROOT files also will be placed there but the debug plots will appear in the build directory.