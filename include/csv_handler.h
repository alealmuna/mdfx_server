#ifndef INCLUDE_CSV_HANDLER_H_
#define INCLUDE_CSV_HANDLER_H_
#include <string>
#include <vector>
#include "include/constants.h"

class CsvHandler {
  public:
    std::vector <Quote> readcsv(std::vector<std::string> files);
    std::vector<std::string> readdir(std::string);
    void sortvec (std::vector <Quote> &);  
  //private:
    void decompress_command(std::string);
    void decompress(std::string);       
    double totstamp(std::string date, std::string hrs);
    std::string fixdate(std::string date, std::string hrs);
};

#endif  // INCLUDE_CSV_HANDLER_H_
