
// createEDP.cpp
// purpose - given a building, return an EVENT for the Haywired data.
// written: fmckenna

#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>

#include <jansson.h>     // for writing json
#include <nanoflann.hpp> // for searching for nearest point

int main(int argc, char **argv) {

  int minRow = atoi(argv[1]);
  int maxRow = atoi(argv[2]);

  ofstream output;
  output.open (argv[3]);

  if (!output.is_open())
    return -1;
    
  for (int i=minRow; i<=maxRow; i++) {
    string inputFilename = to_string(i) + "-DL.json";

    // now parse the DL file for the building
    //
    
    json_error_t error;
    json_t *root = json_load_file(inputFilename.c_str(), 0, &error);
    
    if(!root) {
      output << "0,0,none,0\n";
    } else {

      double loss = 0.;
      double downtime = 0.;
      double prob =0.;
      const char *placard = "none";

      json_t *lossO = json_object_get(root,"EconomicLoss"); 
      if (lossO != 0) {
	loss = json_real_value(json_object_get(lossO,"MedianLossRatio"));
      }

      json_t *downtimeO = json_object_get(root,"Downtime");
      if (downtimeO != 0)
	downtime = json_real_value(json_object_get(downtimeO,"MedianDowntime"));

      json_t *placardsO = json_object_get(root,"UnsafePlacards");
      if (placardsO != 0) {
	placard = json_string_value(json_object_get(placardsO,"Tag"));
	prob = json_real_value(json_object_get(placardsO,"RedTagProbability"));
      }

      
      output << loss << "," << downtime << "," << placard << "," << prob << "\n";

      json_object_clear(root);  
    }
  }

  output.close();

  return 0;
}
