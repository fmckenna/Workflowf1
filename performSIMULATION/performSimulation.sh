#!/bin/bash

# set filenames
filenameBIM = $1
filenameSAM = $2
filenameEVENT = $3
filenameEDP = $4

# perform simulation
./mainPreprocessor $1 $2 $3 $4 example.tcl
OpenSees example.tcl
./mainPostprocessor $1 $2 $3 $4 
