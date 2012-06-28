#ifndef INCLUDE_CSV_HANDLER_H_
#define INCLUDE_CSV_HANDLER_H_
#include <string>
#include <vector>

using std::string;
using std::vector;

class CsvHandler {
  public:
    vector < vector<string> > Readcsv(vector<string> files);
    vector<string> Readdir();
    vector < vector<string> > Sortvec (vector < vector<string> > vec); 
};

#endif  // INCLUDE_CSV_HANDLER_H_
