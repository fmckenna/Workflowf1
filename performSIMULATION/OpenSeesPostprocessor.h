#ifndef OPENSEES_POSTPROCESSOR_H
#define OPENSEES_POSTPROCESSOR_H
class json_t;
#include <fstream>
using namespace::std;

class OpenSeesPostprocessor {

 public:
  OpenSeesPostprocessor();
  ~OpenSeesPostprocessor();

  int processResults(const char *EDP,const char *BIM);

  int processEDPs();

 private:
  char *filenameEDP;
  char *filenameBIM;

  json_t *rootEDP;
  json_t *rootBIM;
};

#endif // OPENSEES_POSTPROCESSOR_H
