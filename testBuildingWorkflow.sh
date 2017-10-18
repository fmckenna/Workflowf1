#!/bin/bash

make
cd createBIM
rm *.json
./createBIM 
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
cd ..

cd createDL
rm example*.json
cp ../performSIMULATION/exampleBIM.json ./
cp ../performSIMULATION/exampleEDP.json ./
./createLOSS exampleBIM.json exampleEDP.json exampleLOSS.json

cat exampleLOSS.json
