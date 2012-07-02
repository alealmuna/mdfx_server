#ifndef INCLUDE_CSV_HANDLER_H_
#define INCLUDE_CSV_HANDLER_H_
#include <string>
#include <vector>
#include "include/constants.h"

class CsvHandler {
  public:
    void decompress(char *dir);
    std::vector <Quote> readcsv(std::vector<std::string> files);
    std::string fixdate(std::string date, std::string hrs);
    double totstamp(std::string date, std::string hrs);
    std::vector<std::string> readdir(std::string);
    void sortvec (std::vector <Quote> &); 
};

#endif  // INCLUDE_CSV_HANDLER_H_
