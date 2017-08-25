
#include "OpenSeesPostprocessor.h"
#include <jansson.h> 
#include <string.h>
#include <string>
#include <sstream>
#include <vector>

OpenSeesPostprocessor::OpenSeesPostprocessor()
  :filenameEDP(0)
{

}

OpenSeesPostprocessor::~OpenSeesPostprocessor(){
  if (filenameEDP != 0)
    delete [] filenameEDP;
}

int 
OpenSeesPostprocessor::processResults(const char *EDP)
{
  //
  // make copies of filenames in case methods need them
  //
  
  if (filenameEDP != 0)
    delete [] filenameEDP;

  filenameEDP=(char*)malloc((strlen(EDP)+1)*sizeof(char));
  strcpy(filenameEDP,EDP);

  json_error_t error;
  rootEDP = json_load_file(filenameEDP, 0, &error);
  
  processEDPs();

  json_dump_file(rootEDP,filenameEDP,0);
  json_object_clear(rootEDP);  
}


int 
OpenSeesPostprocessor::processEDPs(){

  //
  // foreach EVENT
  //   processEDPs, i.e. open ouputfile, read data, write to edp and dump
  //

  int numTimeSeries = 1;
  int numPatterns = 1;

  int index;
  json_t *event;

  json_t *edps = json_object_get(rootEDP,"EngineeringDemandParameters");  
  
  int numEvents = json_array_size(edps);

  for (int i=0; i<numEvents; i++) {

    // process event
    json_t *eventEDPs = json_array_get(edps,i);
    const char *eventName = json_string_value(json_object_get(eventEDPs,"name"));

    // loop through EDPs
    for (int j=0; j<numEvents; j++) {

      json_t *eventEDPs = json_array_get(edps, j);
      const char *edpEventName = json_string_value(json_object_get(eventEDPs,"name"));
      
      json_t *eventEDP = json_object_get(eventEDPs,"responses");
      int numResponses = json_array_size(eventEDP);
      for (int k=0; k<numResponses; k++) {

	json_t *response = json_array_get(eventEDP, k);
	const char *type = json_string_value(json_object_get(response, "type"));

	if (strcmp(type,"max_abs_acceleration") == 0) {
	  int cline = json_integer_value(json_object_get(response, "cline"));
	  int floor = json_integer_value(json_object_get(response, "floor"));
	  
	  string fileString;
	  ostringstream temp;  //temp as in temporary
	  temp << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	  fileString=temp.str(); 

	  const char *fileName = fileString.c_str();
	    
	  // openfile & process data
	  ifstream myfile;
	  myfile.open (fileName);
	  double absMin, absMax, absValue;

	  if (myfile.is_open()) {
	    myfile >> absMin >> absMax >> absValue;
	    printf("%f %f %f\n",absMin,absMax, absValue);
	    myfile.close();
	  }
	  
	  json_object_set(response,"scalar_data",json_real(absValue));

	} else if (strcmp(type,"max_drift") == 0) {
	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor1 = json_integer_value(json_object_get(response, "floor1"));
	    int floor2 = json_integer_value(json_object_get(response, "floor1"));

	    string fileString;
	    ostringstream temp;  //temp as in temporary
	    temp << edpEventName << "." << type << "." << cline << "." 
		 << floor1 << "." << floor2 << ".out";
	    fileString=temp.str(); 

	    const char *fileName = fileString.c_str();

	    // openfile & process data
	    ifstream myfile;
	    myfile.open (fileName);
	    double absMin, absMax, absValue;

	    if (myfile.is_open()) {
	      myfile >> absMin >> absMax >> absValue;
	      printf("%f %f %f\n",absMin,absMax, absValue);
	      myfile.close();
	    }
	    json_object_set(response,"scalar_data",json_real(absValue));
	}

	else if (strcmp(type,"residual_disp") == 0) {
	  int cline = json_integer_value(json_object_get(response, "cline"));
	  int floor = json_integer_value(json_object_get(response, "floor"));
	  
	  string fileString;
	  ostringstream temp;  //temp as in temporary
	  temp << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	  fileString=temp.str(); 

	  const char *fileName = fileString.c_str();
	    
	  // openfile & process data
	  ifstream myfile;
	  myfile.open (fileName);
	  double num = 0.0;
	  if (myfile.is_open()) {
	    std::vector<double> scores;
	    //keep storing values from the text file so long as data exists:
	    while (myfile >> num) {
	      scores.push_back(num);
	    }
	    
	    // need to process to get the right value, for now just output last
	    myfile.close();
	  }
	  
	  json_object_set(response,"scalar_data",json_real(num));
	}
      }
    }
  }
}


