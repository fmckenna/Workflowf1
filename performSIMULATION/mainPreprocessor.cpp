#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "OpenSeesPreprocessor.h"

int main(int argc, char **argv)
{
  if (argc != 6) {
    printf("ERROR: correct usage: preprocessOpenSees fileNameBIM fileNameSAM fileNameEVENT filenameEDP filnameTCL\n");
    exit(0);
  }

  char *filenameBIM = argv[1];
  char *filenameSAM = argv[2];
  char *filenameEVENT = argv[3];
  char *filenameEDP = argv[4];
  char *filenameTCL = argv[5];

  OpenSeesPreprocessor *thePreprocessor = new OpenSeesPreprocessor();
  thePreprocessor->createInputFile(filenameBIM, 
				   filenameSAM, 
				   filenameEVENT,
				   filenameEDP,
				   filenameTCL);

  delete thePreprocessor;
  return 0;
}

