#ifdef OLD_HEADER_FILENAME
#include <iostream.h>
#else
#include <iostream>
#endif
#include <string>
#include <vector>
#include <time.h>
#include <include/constants.h>

#ifndef H5_NO_NAMESPACE
#ifndef H5_NO_STD
    using std::cout;
    using std::endl;
    using std::vector;
    using std::string;
#endif  // H5_NO_STD
#endif

#include "H5Cpp.h"
#include "include/hdf5_handler.h"

#ifndef H5_NO_NAMESPACE
    using namespace H5;
#endif

string getFilename( vector <Quote> quotes_v ){
    struct tm * ptm;
    double mtimestamp;
    char buf[20];
    time_t rawtime;

    Quote* quotes =  &quotes_v[0];
    mtimestamp = quotes[0].tstamp;
    mtimestamp =(int) mtimestamp/86000;
    sprintf(buf,"%d.h5",(int) mtimestamp);
    return buf;
}

void filter( double start, double end, vector <Quote> &result){
  struct request{
    double begin_ts;
    double end_ts;
    double max_rel_spread;
    char *nemo;
  } fxrequest;

  long int begin_index;
  long int end_index;

  fxrequest.begin_ts = start;
  fxrequest.end_ts = end;
  fxrequest.max_rel_spread = 100000; 

  begin_index = fxrequest.begin_ts/86000;
  end_index = fxrequest.end_ts/86000;


  Exception::dontPrint();

  CompType mtype1( sizeof(Quote) );
  mtype1.insertMember( TSTAMP, HOFFSET(Quote, tstamp), PredType::NATIVE_FLOAT);
  mtype1.insertMember( NEMO, HOFFSET(Quote, nemo), PredType::NATIVE_INT);
  mtype1.insertMember( BIDP, HOFFSET(Quote, bidp), PredType::NATIVE_FLOAT);
  mtype1.insertMember( BIDS, HOFFSET(Quote, bids), PredType::NATIVE_FLOAT);
  mtype1.insertMember( ASKP, HOFFSET(Quote, askp), PredType::NATIVE_FLOAT);
  mtype1.insertMember( ASKS, HOFFSET(Quote, asks), PredType::NATIVE_FLOAT);

  DataSet* dataset;
  DataSpace* dataspace;
  hsize_t dims_out[1];

  for (int i = begin_index; i <= end_index; i++){
    char filename[10];
    sprintf(filename,"%d.h5",(int) i);
    try{

      H5std_string FILE_NAME(filename);
      H5File* file = new H5File( FILE_NAME, H5F_ACC_RDONLY );

      dataset = new DataSet (file->openDataSet( DATASET_NAME ));
      dataspace = new DataSpace (dataset->getSpace());

      int ndims = dataspace->getSimpleExtentDims( dims_out, NULL);
      int data_size = dims_out[0];

      Quote quotes[data_size];
      dataset->read( quotes, mtype1 );
      cout.precision(20);

      for( int j = 0; j < data_size; j++){
        if (((i == begin_index) and (quotes[j].tstamp >= start)) or
           ((i == end_index)   and (quotes[j].tstamp <= end )) or
           ( i> begin_index and i < end_index ));
            result.push_back(quotes[j]);
      }
      delete dataset;
      delete file;
    }catch( FileIException error ){
    }
  }
}

int writeToH5( vector <Quote> quotes_v){
  
   try
   {
      int length = quotes_v.size();
      Quote* quotes =  &quotes_v[0];
      Exception::dontPrint();
      /*
       * Create the data space.
       */
      hsize_t dim[] = {length};   /* Dataspace dimensions */
      DataSpace space( RANK, dim );

      /*
       * Create the file.
       */
      H5std_string FILE_NAME(getFilename(quotes_v));
      H5File* file = new H5File( FILE_NAME, H5F_ACC_TRUNC );

      /*
       * Create the memory datatype.
       */
      CompType mtype1( sizeof(Quote) );
      mtype1.insertMember( TSTAMP, HOFFSET(Quote, tstamp), PredType::NATIVE_FLOAT);
      mtype1.insertMember( NEMO, HOFFSET(Quote, nemo), PredType::NATIVE_INT);
      mtype1.insertMember( BIDP, HOFFSET(Quote, bidp), PredType::NATIVE_FLOAT);
      mtype1.insertMember( BIDS, HOFFSET(Quote, bids), PredType::NATIVE_FLOAT);
      mtype1.insertMember( ASKP, HOFFSET(Quote, askp), PredType::NATIVE_FLOAT);
      mtype1.insertMember( ASKS, HOFFSET(Quote, asks), PredType::NATIVE_FLOAT);

      /*
       * Create the dataset.
       */
      DataSet* dataset;
      dataset = new DataSet(file->createDataSet(DATASET_NAME, mtype1, space));

      /*
       * Write data to the dataset;
       */
      dataset->write( quotes, mtype1 );

      /*
       * Release resources
       */
      delete dataset;
      delete file;
   }  // end of try block

   // catch failure caused by the H5File operations
   catch( FileIException error )
   {
      error.printError();
      return -1;
   }

   // catch failure caused by the DataSet operations
   catch( DataSetIException error )
   {
      error.printError();
      return -1;
   }

   // catch failure caused by the DataSpace operations
   catch( DataSpaceIException error )
   {
      error.printError();
      return -1;
   }

   // catch failure caused by the DataSpace operations
   catch( DataTypeIException error )
   {
      error.printError();
      return -1;
   }
}

int readFromH5( vector <Quote> &result ){
   try
   {

      Exception::dontPrint();
      H5std_string FILE_NAME("5011.h5");
      H5File* file = new H5File( FILE_NAME, H5F_ACC_RDONLY );

      CompType mtype1( sizeof(Quote) );
      mtype1.insertMember( TSTAMP, HOFFSET(Quote, tstamp), PredType::NATIVE_FLOAT);
      mtype1.insertMember( NEMO, HOFFSET(Quote, nemo), PredType::NATIVE_INT);
      mtype1.insertMember( BIDP, HOFFSET(Quote, bidp), PredType::NATIVE_FLOAT);
      mtype1.insertMember( BIDS, HOFFSET(Quote, bids), PredType::NATIVE_FLOAT);
      mtype1.insertMember( ASKP, HOFFSET(Quote, askp), PredType::NATIVE_FLOAT);
      mtype1.insertMember( ASKS, HOFFSET(Quote, asks), PredType::NATIVE_FLOAT);

      DataSet* dataset;
      DataSpace* dataspace;
      hsize_t dims_out[1];

      dataset = new DataSet (file->openDataSet( DATASET_NAME ));
      dataspace = new DataSpace (dataset->getSpace());

      int ndims = dataspace->getSimpleExtentDims( dims_out, NULL);
      int data_size = dims_out[0];

      Quote quotes[data_size];
      dataset->read( quotes, mtype1 );

      cout.precision(20);
      for( int i = 0; i < data_size; i++){
         cout << "QUOTE " << i+1 << endl; 
         cout << "\t timestamp:\t  " << quotes[i].tstamp << endl;
         cout << "\t nemo:\t\t  " << quotes[i].nemo << endl;
         cout << "\t bid_price:\t  " << quotes[i].bidp << endl;
         cout << "\t bid_size:\t  " << quotes[i].bids << endl;
         cout << "\t ask_price:\t  " << quotes[i].askp << endl;
         cout << "\t ask_size:\t  " << quotes[i].asks << endl;
         cout << endl;
         result.push_back(quotes[i]);
      }
      delete dataset;
      delete file;
   }

   catch( FileIException error )
   {
      error.printError();
      return -1;
   }

   catch( DataSetIException error )
   {
      error.printError();
      return -1;
   }

   catch( DataSpaceIException error )
   {
      error.printError();
      return -1;
   }

   catch( DataTypeIException error )
   {
      error.printError();
      return -1;
   }
}

int main(void)
{
  int  i;
  const int length = 4;
  
  //Quote quotes[length];
  vector <Quote> quotes;
  vector <Quote> iquotes;
  vector <Quote> fquotes;
  Quote quote;

  double raw_quotes[length][6] = { 
    {430986918,1, 1.60469, 1000000, 1.60496, 1000000},
    {134091528,2, 80.51000, 2000000, 80.52400, 2000000},
    {430986918,3, 1.45076, 1000000, 1.45082, 1000000},
    {430986981,1, 1.60468, 1000000, 1.60495, 1000000} 
  };

  for (i = 0; i< length; i++)
  {
     quote.tstamp = raw_quotes[i][0];
     quote.nemo = raw_quotes[i][1];
     quote.bidp = raw_quotes[i][2];
     quote.bids = raw_quotes[i][3]; 
     quote.askp = raw_quotes[i][4];
     quote.asks = raw_quotes[i][5];
     quotes.push_back(quote);
  }

  writeToH5(quotes);
  readFromH5(iquotes);
  filter(180956917,430986918,fquotes);
  return 0;
}
