// createBIM.c
// purpose - give the building.csv and parcel.csv files create a BIM model
// written: fmckenna

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "csvparser.h"
#include <jansson.h>
#include <map>
#include <sstream>
#include <fstream>
using namespace std;
const char* deterStructtype(int year, int bldtypeid, int story);
const char* deteroccupancy(int building_type);
double replacementcost(int building_type);

struct locations {
  locations():x(0),y(0) {}
  locations(double a,double b):x(a),y(b) {}
  double x;
  double y;
};
const char* deteroccupancy(int building_type){
    const char * buildoccupancy;
    //double replacementcost;
    switch (building_type) {
    case 1:
        buildoccupancy="Residential";
        break;
    case 2:
        buildoccupancy="Residential";
        break;
    case 3:
        buildoccupancy="Residential";
        break;
    case 4:
        buildoccupancy="Office";
        break;
    case 5:
        buildoccupancy="Hotel";
        break;
    case 6:
        buildoccupancy="School";
        break;
    case 7:
        buildoccupancy="Industrial";
        break;
    case 8:
        buildoccupancy="Industrial";
        break;
    case 9:
        buildoccupancy="Industrial";
        break;
    case 10:
        buildoccupancy="Retail";
        break;
    case 11:
        buildoccupancy="Retail";
        break;
    case 12:
        buildoccupancy="Residential";
        break;
    case 13:
        buildoccupancy="Retail";
        break;
    case 14:
        buildoccupancy="Office";
        break;
    default:
        buildoccupancy="Residential";
        break;
    }
    return buildoccupancy;
}
double replacementcost(int building_type){
    //const char * buildoccupancy;
    double replacementcost;
    switch (building_type) {
    case 1:
        replacementcost=137.5452*(1+0.5);
        break;
    case 2:
        replacementcost=137.5452*(1+0.5);
        break;
    case 3:
        replacementcost=137.5452*(1+0.5);
        break;
    case 4:
        replacementcost=131.8863*(1+1);
        break;
    case 5:
        replacementcost=137.271225*(1+0.5);
        break;
    case 6:
        replacementcost=142.134265*(1+1.25);
        break;
    case 7:
        replacementcost=97.5247*(1+1.5);
        break;
    case 8:
        replacementcost=85.9586*(1+1.5);
        break;
    case 9:
        replacementcost=104.033475*(1+1.5);
        break;
    case 10:
        replacementcost=105.33705*(1+1);
        break;
    case 11:
        replacementcost=105.33705*(1+1);
        break;
    case 12:
        replacementcost=137.5452*(1+0.5);
        break;
    case 13:
        replacementcost=105.33705*(1+1);
        break;
    case 14:
        replacementcost=131.8863*(1+1);
        break;
    default:
        replacementcost=137.5452*(1+0.5);
        break;
    }
    return replacementcost;
}
const char* deterStructtype(int year, int bldtypeid, int story)
{
    const char* buildingtype;
    int structingtypeID;
    if(year<=1990){
        int rand_array[4]={1,7,8,9};
        structingtypeID=rand_array[(rand() % (3-0+1))+ 0];
    }
    else if(year>1990){
        if(story<4){
            if(bldtypeid<13){  //1-12 Residential 13-19 Commercial 20-22 Industrial 23-25 Other
                structingtypeID=1;
            }
            else if(bldtypeid<20&&bldtypeid>12){
                structingtypeID=(rand() % (8-1+1))+ 1; //Commercial
            }
            else if(bldtypeid>20&&bldtypeid<23){
                structingtypeID=(rand() % (8-2+1))+ 2; //Industrial
            }
            else {
                structingtypeID=1;
            }

        }
        else if(story>3&&story<8){
            if(bldtypeid<20){  //1-12 Residential 13-19 Commercial 20-22 Industrial 23-25 Other
                structingtypeID=(rand() % (5-1+1))+ 1;//Commercial and Residential
            }
            else if(bldtypeid>20&&bldtypeid<23){
                structingtypeID=(rand() % (5-2+1))+ 2; //Industrial
            }
            else {
                structingtypeID=(rand() % (5-1+1))+ 1;//other
            }

        }
        else {
            structingtypeID=(rand() % (5-2+1))+ 2;//other
        }


    }
    switch (structingtypeID) {
    case 1:
        buildingtype="W1";
        break;
    case 2:
        buildingtype="S1";
        break;
    case 3:
        buildingtype="S2";
        break;
    case 4:
        buildingtype="C1";
        break;
    case 5:
        buildingtype="C2";
        break;
    case 6:
        buildingtype="C3";
        break;
    case 7:
        buildingtype="RM1";
        break;
    case 8:
        buildingtype="RM2";
        break;
    case 9:
        buildingtype="URM";
        break;
    default:
        break;
    }

    return buildingtype;
}
//double replacementcost(int building_type,double area)
int main(int argc, const char **argv) {

  int minRow = 10;
  int maxRow = 10;

  if (argc == 3) {
    minRow = atoi(argv[1]);
    maxRow = atoi(argv[2]);
  }

  int i =  0;

  std::map<int, locations> parcelLocations;
  std::map<int, locations>::iterator parcelIter;
  //ofstream opt("ID.txt");
  //system("md fileBIM");
  //
  // first parse the parcel file, storing parcel location in a map
  //

  CsvParser *csvparser = CsvParser_new("parcels.csv", ",", 1);
  const CsvRow *header = CsvParser_getHeader(csvparser);
  
  if (header == NULL) {
    printf("%s\n", CsvParser_getErrorMessage(csvparser));
    return 1;
  }
  
  const char **headerFields = CsvParser_getFields(header);
  for (i = 0 ; i < CsvParser_getNumFields(header) ; i++) {
    ;//    printf("TITLE: %d %s\n", i, headerFields[i]);
  }    
  CsvRow *row;
  
  while ((row = CsvParser_getRow(csvparser))) {
    const char **rowFields = CsvParser_getFields(row);
    for (i = 0 ; i < CsvParser_getNumFields(row) ; i++) {
      ;//   printf("FIELD: %d %s\n", i, rowFields[i]);
    }
    char *pEnd;
    int parcelID = atoi(rowFields[0]);
    double x = strtod(rowFields[12],&pEnd);
    double y = strtod(rowFields[13],&pEnd);
    parcelLocations[parcelID]=locations(x,y);
  }
  
  CsvParser_destroy(csvparser);
  
  //
  // now parse the building file, obtaining location form parcel info
  // writing and write a BIM file
  //
  
  csvparser = CsvParser_new("buildings.csv", ",", 1);
  header = CsvParser_getHeader(csvparser);
  
  if (header == NULL) {
    printf("%s\n", CsvParser_getErrorMessage(csvparser));
    return 1;
  }
  
  headerFields = CsvParser_getFields(header);
  for (i = 0 ; i < CsvParser_getNumFields(header) ; i++) {
    ; //        printf("TITLE: %d %s\n", i, headerFields[i]);
  }
  
  int currentRow = 1;
  
  json_t *root = json_object();
  
  while ((row = CsvParser_getRow(csvparser))) {
    if (currentRow >= minRow && currentRow <= maxRow) {
      const char **rowFields = CsvParser_getFields(row);
       for (i = 0 ; i < CsvParser_getNumFields(row) ; i++) {
      printf("FIELD: %d %s\n", i, rowFields[i]);
      }
      //string filename1;
      //stringstream ss;
      //ss.clear();
      //ss.str("");
      
      char *pEnd;

      json_t *GI = json_object();
      const char *name = rowFields[0];
      int numStory = atoi(rowFields[10]);
      double area=strtod(rowFields[8],&pEnd)/10.764/(double)numStory;
      if (area<=0) area=20;
      json_object_set(GI,"area",json_real(area));
      //json_object_set(GI,"structType",json_string("UNKNOWN"));
      json_object_set(GI,"structType",json_string(deterStructtype(atoi(rowFields[11]),atoi(rowFields[2]),atoi(rowFields[10]))));
      json_object_set(GI,"name",json_string(name));
      //json_object_set(GI,"area",json_real(strtod(rowFields[8],&pEnd)));
      json_object_set(GI,"numStory",json_integer(numStory));
      json_object_set(GI,"yearBuilt",json_integer(atoi(rowFields[11])));
      
      // unknown
      //json_object_set(GI,"occupancy",json_string("office"));
      //deteroccupancy(atoi(rowFields[15]), buildoccupancy, replacementcost);
      json_object_set(GI,"occupancy",json_string(deteroccupancy(atoi(rowFields[15]))));

      json_object_set(GI,"height",json_real(3.0*numStory));
      json_object_set(GI,"replacementCost",json_real(replacementcost(atoi(rowFields[15]))*strtod(rowFields[8],&pEnd)));
      json_object_set(GI,"replacementTime",json_real(180.0));
      //json_object_set(GI,"structType",json_string("C2"));
      
      int parcelID = atoi(rowFields[1]);
      parcelIter = parcelLocations.find(parcelID);
      if (parcelIter != parcelLocations.end()) {
	//	double x = parcelIter->second.x;
	json_t *location = json_object();
	json_object_set(location,"latitude",json_real(parcelIter->second.y));
	json_object_set(location,"longitude",json_real(parcelIter->second.x));
	json_object_set(GI,"location",location);
      }
      
      json_object_set(root,"GI",GI);
      std::string filename;
      filename = "exampleBIM.json";

      if (argc > 1) {
	filename = std::string(name) + std::string("-BIM.json");
      }
	
      // write the file & clean memory
      json_dump_file(root,filename.c_str(),0);
      
      //opt<<parcelID<<endl;
      //ss<<"fileBIM\\"<<parcelID<<"-BIM.json";
      //filename1=ss.str().c_str();
      //const char *BIMname = filename1.c_str();
      //json_dump_file(root,BIMname,0);

      json_object_clear(root);
      CsvParser_destroy_row(row);
    }


    currentRow++;
    
    if (currentRow > maxRow)
      break;
  }
  
  CsvParser_destroy(csvparser);
  
  return 0;
}
