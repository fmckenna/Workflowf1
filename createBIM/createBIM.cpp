// createBIM.c
// purpose - give the building.csv and parcel.csv files create a BIM model
// written: fmckenna

#include <stdio.h>
#include <stdlib.h>

#include "csvparser.h"
#include <jansson.h>
#include <map>


struct locations {
  locations():x(0),y(0) {}
  locations(double a,double b):x(a),y(b) {}
  double x;
  double y;
};


int main() {
    int i =  0;

    std::map<int, locations> parcelLocations;
    std::map<int, locations>::iterator parcelIter;
    
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
      printf("TITLE: %d %s\n", i, headerFields[i]);
    }    
    CsvRow *row;

    while ((row = CsvParser_getRow(csvparser))) {
      const char **rowFields = CsvParser_getFields(row);
      // for (i = 0 ; i < CsvParser_getNumFields(row) ; i++) {
      //   printf("FIELD: %s\n", rowFields[i]);
      // }
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
      //      printf("TITLE: %d %s\n", i, headerFields[i]);
    }

    int currentRow = 0;

    json_t *root = json_object();

    while ((row = CsvParser_getRow(csvparser)) && currentRow < 10) {
      const char **rowFields = CsvParser_getFields(row);
      // for (i = 0 ; i < CsvParser_getNumFields(row) ; i++) {
      //   printf("FIELD: %s\n", rowFields[i]);
      // }

      char *pEnd;
      json_t *GI = json_object();
      json_object_set(GI,"structType",json_string("UNKNOWN"));
      json_object_set(GI,"area",json_real(strtod(rowFields[8],&pEnd)));
      json_object_set(GI,"numStory",json_integer(atoi(rowFields[10])));
      json_object_set(GI,"height",json_real(1.0));
      json_object_set(GI,"yearBuilt",json_integer(atoi(rowFields[11])));

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

      // write the file & clean memory
      json_dump_file(root,"exampleBIM.json",0);

      json_object_clear(root);
      CsvParser_destroy_row(row);
      currentRow++;
    }

    CsvParser_destroy(csvparser);
	
    return 0;
}
