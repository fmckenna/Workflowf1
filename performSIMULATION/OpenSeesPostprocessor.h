#ifndef OPENSEES_POSTPROCESSOR_H
#define OPENSEES_POSTPROCESSOR_H
class json_t;
#include <fstream>
using namespace::std;

class OpenSeesPostprocessor {

 public:
  OpenSeesPostprocessor();
  ~OpenSeesPostprocessor();

  int processResults(const char *EDP);

  int processEDPs();

 private:
  char *filenameEDP;

  json_t *rootEDP;
};

#endif // OPENSEES_POSTPROCESSOR_H
