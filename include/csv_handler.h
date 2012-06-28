#ifndef INCLUDE_CSV_HANDLER_H_
#define INCLUDE_CSV_HANDLER_H_
#include <string>
#include <vector>

using std::string;
using std::vector;

class CsvHandler {
  public:
    vector < vector<string> > readcsv(vector<string> files);
    vector<string> readdir();
    vector < vector<string> > sortvec (vector < vector<string> > vec); 
};

#endif  // INCLUDE_CSV_HANDLER_H_
