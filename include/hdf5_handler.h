#ifndef INCLUDE_HDF5_HANDLER_H_
#define INCLUDE_HDF5_HANDLER_H_
#include <vector>
#include <string>

const H5std_string FILE_NAME( "MDFXQuotes.h5" );
const H5std_string DATASET_NAME( "Market data FX Quotes" );
const H5std_string TSTAMP( "timestamp" );
const H5std_string NEMO( "symbol_nemo" );
const H5std_string BIDP( "bid_price" );
const H5std_string BIDS( "bid_size" );
const H5std_string ASKP( "ask_price" );
const H5std_string ASKS( "ask_size" );

const int   RANK = 1;

int writeToH(vector <Quote>);
int readFromH5( vector <Quote> &);
#endif
