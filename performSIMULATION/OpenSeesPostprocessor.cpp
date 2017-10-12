
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
OpenSeesPostprocessor::processResults(const char *EDP,const char *BIM)
{
  //
  // make copies of filenames in case methods need them
  //
  
  if (filenameEDP != 0)
    delete [] filenameEDP;

  filenameEDP=(char*)malloc((strlen(EDP)+1)*sizeof(char));
  strcpy(filenameEDP,EDP);
  filenameBIM=(char*)malloc((strlen(BIM)+1)*sizeof(char));
  strcpy(filenameBIM,BIM);

  json_error_t error;
  rootEDP = json_load_file(filenameEDP, 0, &error);
  rootBIM = json_load_file(filenameBIM, 0, &error);
  
  processEDPs();

  json_dump_file(rootEDP,filenameEDP,0);
  json_object_clear(rootEDP);  
  json_object_clear(rootBIM);
  return 0;
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
  json_t *GI = json_object_get(rootBIM,"GI");
  json_t *nType = json_object_get(GI,"numStory");
  json_t *hType = json_object_get(GI,"height");
  int nStory=json_integer_value(nType);
  double storyheight=json_number_value(hType)/(nStory*1.);

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
      double absMinX,absMinY,absMaxX,absMaxY,absValueX,absValueY;

	  if (myfile.is_open()) {
        myfile >> absMinX>>absMinY>>absMaxX>>absMaxY>>absValueX>>absValueY;
        printf("%f %f %f\n",absMinX,absMinY,absMaxX,absMaxY,absValueX,absValueY);
	    myfile.close();
	  }
      if(absValueX<absValueY){absValueX=absValueY;}
	  
      json_object_set(response,"scalar_data",json_real(absValueX));

	} else if (strcmp(type,"max_drift") == 0) {
	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor1 = json_integer_value(json_object_get(response, "floor1"));
        int floor2 = json_integer_value(json_object_get(response, "floor2"));

	    string fileString;
	    ostringstream temp;  //temp as in temporary
	    temp << edpEventName << "." << type << "." << cline << "." 
		 << floor1 << "." << floor2 << ".out";
	    fileString=temp.str(); 

	    const char *fileName = fileString.c_str();

	    // openfile & process data
	    ifstream myfile;
	    myfile.open (fileName);
        double absMinX,absMinY,absMaxX,absMaxY,absValueX,absValueY;

	    if (myfile.is_open()) {
          //myfile >> absMinX>>absMinY>>absMaxX>>absMaxY>>absValueX>>absValueY;
          myfile >> absMinX>>absMaxX>>absValueX;
          printf("%f %f %f\n",absMinX,absMaxX,absValueX);
	      myfile.close();
	    }
        //if(absValueX<absValueY){absValueX=absValueY;}
        json_object_set(response,"scalar_data",json_real(absValueX));
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
      double **disp;
      disp=new double*[2];//x、y direction
      for( i = 0 ; i < 2 ; i ++ )
         disp[i] = new double[floor];

      int line=-1;//count line number
      while(!myfile.eof()){
          for(int i=0;i<floor;i++){myfile>>disp[0][i]>>disp[1][i];}
          line++;
      }
      //ofstream opt("check.txt");
      //opt<<line<<endl;
      //for(int i=0;i<floor;i++)
      //{opt<<disp[0][i]<<" "<<disp[1][i];}
      double residualx=0;
      double residualy=0;
      for(int i=0;i<floor-1;i++){
          if(residualx<abs(disp[0][i+1]-disp[0][i])/storyheight){
              residualx=abs(disp[0][i+1]-disp[0][i])/storyheight;
          }
          if(residualy<abs(disp[1][i+1]-disp[1][i])/storyheight){
              residualy=abs(disp[1][i+1]-disp[1][i])/storyheight;
          }

      }
      if(residualx<residualy){residualx=residualy;}


/*	  if (myfile.is_open()) {
	    std::vector<double> scores;
	    //keep storing values from the text file so long as data exists:
	    while (myfile >> num) {
	      scores.push_back(num);
	    }
	    
	    // need to process to get the right value, for now just output last
	    myfile.close();
	  }
*/
      json_object_set(response,"scalar_data",json_real(residualx));
	}
      }
    }
  }
  return 0;
}


