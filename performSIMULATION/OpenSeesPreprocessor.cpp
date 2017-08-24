
#include "OpenSeesPreprocessor.h"
#include <jansson.h> 
#include <string.h>
#include <string>
#include <sstream>

OpenSeesPreprocessor::OpenSeesPreprocessor()
  :filenameBIM(0),filenameSAM(0),filenameEVENT(0),filenameEDP(0),
   filenameTCL(0), analysisType(-1), numSteps(0), dT(0.0)
{

}

OpenSeesPreprocessor::~OpenSeesPreprocessor(){
  if (filenameBIM != 0)
    delete [] filenameBIM;
  if (filenameSAM != 0)
    delete [] filenameSAM;
  if (filenameEVENT != 0)
    delete [] filenameEVENT;
  if (filenameEDP != 0)
    delete [] filenameEDP;
  if (filenameTCL != 0)
    delete [] filenameTCL;
}



int 
OpenSeesPreprocessor::createInputFile(const char *BIM,
				      const char *SAM,
				      const char *EVENT,
				      const char *EDP,
				      const char *tcl)
{
  //
  // make copies of filenames in case methods need them
  //

  if (filenameBIM != 0)
    delete [] filenameBIM;
  if (filenameSAM != 0)
    delete [] filenameSAM;
  if (filenameEVENT != 0)
    delete [] filenameEVENT;
  if (filenameEDP != 0)
    delete [] filenameEDP;
  if (filenameTCL != 0)
    delete [] filenameTCL;

  filenameBIM=(char*)malloc((strlen(BIM)+1)*sizeof(char));
  filenameSAM=(char*)malloc((strlen(SAM)+1)*sizeof(char));
  filenameEVENT=(char*)malloc((strlen(EVENT)+1)*sizeof(char));
  filenameEDP=(char*)malloc((strlen(EDP)+1)*sizeof(char));
  filenameTCL=(char*)malloc((strlen(tcl)+1)*sizeof(char));
  strcpy(filenameBIM,BIM);
  strcpy(filenameSAM,SAM);
  strcpy(filenameEVENT,EVENT);
  strcpy(filenameEDP,EDP);
  strcpy(filenameTCL,tcl);

  //
  // open tcl script
  // 

  ofstream *s = new ofstream;
  s->open(filenameTCL, ios::out);
  ofstream &tclFile = *s;

  //
  // process the SAM to create the model
  //

  json_error_t error;
  rootSAM = json_load_file(filenameSAM, 0, &error);

  mapping = json_object_get(rootSAM,"NodeMapping");  

  processNodes(tclFile);
  processMaterials(tclFile);
  processElements(tclFile);

  rootEVENT = json_load_file(filenameEVENT, 0, &error);
  rootEDP = json_load_file(filenameEDP, 0, &error);

  processEvents(tclFile);

  s->close();
}

int 
OpenSeesPreprocessor::processMaterials(ofstream &s){
  int index;
  json_t *material;

  json_t *propertiesObject = json_object_get(rootSAM,"Properties");  
  json_t *materials = json_object_get(propertiesObject,"uniaxialMaterials");
  json_array_foreach(materials, index, material) {
    const char *type = json_string_value(json_object_get(material,"type"));
    
    if (strcmp(type,"shear") == 0) {
      int tag = json_integer_value(json_object_get(material,"name"));
      double K0 = json_real_value(json_object_get(material,"K0"));
      double Sy = json_real_value(json_object_get(material,"Sy"));
      double eta = json_real_value(json_object_get(material,"eta"));
      double C = json_real_value(json_object_get(material,"C"));
      double gamma = json_real_value(json_object_get(material,"gamma"));
      double alpha = json_real_value(json_object_get(material,"alpha"));
      double beta = json_real_value(json_object_get(material,"beta"));
      double omega = json_real_value(json_object_get(material,"omega"));
      double eta_soft = json_real_value(json_object_get(material,"eta_soft"));
      double a_k = json_real_value(json_object_get(material,"a_k"));
      s << "uniaxialMaterial Elastic " << tag << " " << K0 << "\n";
    }
  }
}

int 
OpenSeesPreprocessor::processSections(ofstream &s) {

}

int 
OpenSeesPreprocessor::processNodes(ofstream &s){
  int index;
  json_t *node;

  json_t *geometry = json_object_get(rootSAM,"Geometry");  
  json_t *nodes = json_object_get(geometry,"nodes");

  int NDF = 0;
  int NDM = 0;

  json_array_foreach(nodes, index, node) {
    int tag = json_integer_value(json_object_get(node,"name"));
    json_t *crds = json_object_get(node,"crd");
    int sizeCRD = json_array_size(crds);

    if (sizeCRD != NDM) {
      if (sizeCRD == 1) {
	NDM = 1;
	NDF = 1;
      } else if (sizeCRD == 2) {
	NDM = 2;
	NDF = 3;
      } else if (sizeCRD == 3) {
	NDM = 3;
	NDF = 6;
      }
      // issue new model command if node size changes
      s << "model BasicBuilder -ndm " << NDM << " -ndf " << NDF << "\n";
    } 

    s << "node " << tag << " ";
    json_t *crd;
    int crdIndex;
    json_array_foreach(crds, crdIndex, crd) {
      s << json_real_value(crd) << " " ;
    }

    json_t *mass = json_object_get(node,"mass");
    if (mass != NULL) {
      s << "-mass ";
      double massV = json_real_value(mass);
      for (int i=0; i<NDM; i++)
	s << massV << " " ;
    }

    s << "\n";
  }
}

int 
OpenSeesPreprocessor::processElements(ofstream &s){
  int index;
  json_t *element;

  json_t *geometry = json_object_get(rootSAM,"Geometry");  
  json_t *elements = json_object_get(geometry,"elements");

  json_array_foreach(elements, index, element) {

    int tag = json_integer_value(json_object_get(element,"name"));
    const char *type = json_string_value(json_object_get(element,"type"));
    if (strcmp(type,"shear_beam") == 0) {
      s << "element zeroLength " << tag << " " ;
      json_t *nodes = json_object_get(element,"nodes");
      json_t *nodeTag;
      int nodeIndex;
      json_array_foreach(nodes, nodeIndex, nodeTag) {
	s << json_integer_value(nodeTag) << " " ;
      }

      int matTag = json_integer_value(json_object_get(element,"uniaxial_material"));
      s << "-mat " << matTag << " -dir 1 \n";
    }
  }
}

int 
OpenSeesPreprocessor::processEvents(ofstream &s){

  //
  // foreach EVENT
  //   create load pattern
  //   add recorders
  //   add analysis script
  //

  int numTimeSeries = 1;
  int numPatterns = 1;

  int index;
  json_t *event;

  json_t *events = json_object_get(rootEVENT,"Events");  
  json_t *edps = json_object_get(rootEDP,"EngineeringDemandParameters");  
  
  int numEvents = json_array_size(events);
  int numEDPs = json_array_size(edps);

  for (int i=0; i<numEvents; i++) {

    // process event
    json_t *event = json_array_get(events,i);
    processEvent(s,event,numPatterns,numTimeSeries);
    const char *eventName = json_string_value(json_object_get(event,"name"));

    // create recorder foreach EDP
    // loop through EDPs and find corresponding EDP
    for (int j=0; j<numEDPs; j++) {

      json_t *eventEDPs = json_array_get(edps, j);
      const char *edpEventName = json_string_value(json_object_get(eventEDPs,"name"));

      if (strcmp(edpEventName, eventName) == 0) {
	json_t *eventEDP = json_object_get(eventEDPs,"responses");
	int numResponses = json_array_size(eventEDP);
	for (int k=0; k<numResponses; k++) {

	  json_t *response = json_array_get(eventEDP, k);
	  const char *type = json_string_value(json_object_get(response, "type"));
	  if (strcmp(type,"max_abs_acceleration") == 0) {
	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor = json_integer_value(json_object_get(response, "floor"));

	    int nodeTag = this->getNode(cline,floor);	    
	    //	    std::ostringstream fileString(string(edpEventName)+string(type));
	    string fileString;
	    ostringstream temp;  //temp as in temporary
	    temp << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	    fileString=temp.str(); 

	    const char *fileName = fileString.c_str();
	    
	    s << "recorder EnvelopeNode -file " << fileName;
	    s << " -timeSeries "  << numTimeSeries-1;
	    s << " -node " << nodeTag << " -dof 1 accel\n";
	  }

	  else if (strcmp(type,"max_drift") == 0) {
	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor1 = json_integer_value(json_object_get(response, "floor1"));
	    int floor2 = json_integer_value(json_object_get(response, "floor2"));

	    int nodeTag1 = this->getNode(cline,floor1);	    
	    int nodeTag2 = this->getNode(cline,floor2);	    

	    string fileString;
	    ostringstream temp;  //temp as in temporary
	    temp << edpEventName << "." << type << "." << cline << "." << floor1 << "." << floor2 << ".out";
	    fileString=temp.str(); 

	    const char *fileName = fileString.c_str();
	    
	    s << "recorder EnvelopeDrift -file " << fileName;
	    s << " -iNode " << nodeTag1 << " -jNode " << nodeTag2;
	    s << " -perpDirn 1\n";
	  }

	  else if (strcmp(type,"residual_disp") == 0) {

	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor = json_integer_value(json_object_get(response, "floor"));

	    int nodeTag = this->getNode(cline,floor);	    

	    string fileString;
	    ostringstream temp;  //temp as in temporary
	    temp << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	    fileString=temp.str(); 

	    const char *fileName = fileString.c_str();
	    
	    s << "recorder Node -file " << fileName;
	    s << " -node " << nodeTag << " -dof 1 disp\n";
	  }
	}
      }
    }

    // create analysis
    if (analysisType == 1) {
      s << "analysis Transient\n";
      s << "analyze " << numSteps << " " << dT << "\n";
    }
  }
}


// seperate for multi events
int 
OpenSeesPreprocessor::processEvent(ofstream &s, 
				   json_t *event, 
				   int &numPattern, 
				   int &numSeries){

  const char *type = json_string_value(json_object_get(event,"type"));
  const char *subType = json_string_value(json_object_get(event,"subtype"));    
  if (strcmp(subType,"UniformAcceleration") == 0) {
    double dt = json_real_value(json_object_get(event,"dT"));
    json_t *data = json_object_get(event,"data");
    s << "timeSeries Path " << numSeries << " -dt " << dt;
    s << " -values \{ ";
    json_t *dataV;
    int dataIndex;
    json_array_foreach(data, dataIndex, dataV) {
      s << json_real_value(dataV) << " " ;
    }
    s << " }\n";

    int dirn = 1;
    
    s << "pattern UniformExcitation " << numPattern << " " << dirn;
    s << " -accel " << numSeries << "\n";

    analysisType = 1;
    numSteps = json_array_size(data);
    dT = dt;
    printf("%d %d %f\n",analysisType, numSteps, dT);

    numSeries++; 
    numPattern++;

  }  
}


int
OpenSeesPreprocessor:: getNode(int cline, int floor){

  int numMapObjects = json_array_size(mapping);
  for (int i=0; i<numMapObjects; i++) {
    json_t *mapObject = json_array_get(mapping, i); 
    int c = json_integer_value(json_object_get(mapObject,"cline"));
    if (c == cline) {
      int f = json_integer_value(json_object_get(mapObject,"floor"));
      if (f == floor)
	return json_integer_value(json_object_get(mapObject,"node"));
    }
  }
  return -1;
}
