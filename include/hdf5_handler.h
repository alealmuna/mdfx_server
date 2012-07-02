#ifndef INCLUDE_HDF5_HANDLER_H_
#define INCLUDE_HDF5_HANDLER_H_
#include <vector>
#include <string>

#include "H5Cpp.h"
#include "include/constants.h"

const H5std_string FILE_NAME("MDFXQuotes.h5");
const H5std_string DATASET_NAME("Market data FX Quotes");
const H5std_string TSTAMP("timestamp");
const H5std_string NEMO("symbol_nemo");
const H5std_string BIDP("bid_price");
const H5std_string BIDS("bid_size");
const H5std_string ASKP("ask_price");
const H5std_string ASKS("ask_size");
const H5std_string DAY("day");
const H5std_string INDEX("index");

const int   RANK = 1;
const long int SID = 86400;

int writeToH5(std::vector <Quote> &, std::string);
int createIndex(std::vector <Quote> &, std::string);
int writeToH5perDay(std::vector <Quote> &);
int readFromH5(std::vector <Quote> &);
void ProcessResponse(Fxrequest, std::vector <Quote> &);
std::string get_filename( std::vector <Quote> &);
std::string format_filename(double);
#endif  // INCLUDE_HDF5_HANDLER_H_
