#ifndef INCLUDE_CSV_HANDLER_H_
#define INCLUDE_CSV_HANDLER_H_
#include <string>
#include <vector>
#include "include/constants.h"

using std::string;
using std::vector;

class CsvHandler {
  public:
    vector <Quote> readcsv(vector<string> files);
    int fixdate(string date, string hrs);
    vector<string> readdir(char * dir);
    void sortvec (vector <Quote> vec); 
};

#endif  // INCLUDE_CSV_HANDLER_H_
