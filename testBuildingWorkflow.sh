#!/bin/bash

if [ $# -eq 0 ]
  then
    building=0;
  else
    building=$1
fi

make

cd createBIM
rm *.json
if [ $building -eq 0 ]
  then
    ./createBIM 
  else
    ./createBIM $1 $1
    cp $1-BIM.json exampleBIM.json
fi
cd ..

cd createEVENT
rm example*.json
cp ../createBIM/example*.json ./
./createEVENT exampleBIM.json exampleEVENT.json
cd ..

cd createSAM
rm example*.json
cp ../createEVENT/example*.json ./
./createSAM exampleBIM.json exampleEVENT.json exampleSAM.json
cd ..

cd createEDP
rm example*.json
cp ../createSAM/example*.json ./
./createEDP exampleBIM.json exampleSAM.json exampleEVENT.json exampleEDP.json
cd ..

cd performSIMULATION
rm example*.json
rm *.out
cp ../createEDP/example*.json ./
./performSimulation.sh exampleBIM.json exampleSAM.json exampleEVENT.json exampleEDP.json
rm *.out
cd ..

cd createDL
rm example*.json
cp ../performSIMULATION/exampleBIM.json ./
cp ../performSIMULATION/exampleEDP.json ./
./createLOSS exampleBIM.json exampleEDP.json exampleDL.json
cat exampleDL.json
cd ..


cd finalProcessing
rm *.json
cp ../createDL/exampleDL.json ./$1-DL.json
./readDLs $1 $1 test
cat test
rm test *.json

