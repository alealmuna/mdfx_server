#include <string>
#include <vector>
#include <iostream>
#include <time.h>

    using std::cout;
    using std::endl;
    using std::vector;
    using std::string;

#include "H5Cpp.h"
#include "include/hdf5_handler.h"
#include "include/constants.h"

    using namespace H5;

int writeToH5(vector <Quote> &quotes_v, H5std_string FILE_NAME) {
  try {
    int length = quotes_v.size();
    Quote* quotes = &quotes_v[0];
    Exception::dontPrint();
    /*
     * Create the data space.
     */
    hsize_t dim[] = {length};   /* Dataspace dimensions */
    DataSpace space(RANK, dim);

    /*
     * Create the file.
     */
    H5File* file = new H5File(FILE_NAME, H5F_ACC_TRUNC);

    /*
     * Create the memory datatype.
     */
    CompType mtype1(sizeof(Quote));
    mtype1.insertMember(TSTAMP, HOFFSET(Quote, tstamp), PredType::NATIVE_FLOAT);
    mtype1.insertMember(NEMO, HOFFSET(Quote, nemo), PredType::NATIVE_INT);
    mtype1.insertMember(BIDP, HOFFSET(Quote, bidp), PredType::NATIVE_FLOAT);
    mtype1.insertMember(BIDS, HOFFSET(Quote, bids), PredType::NATIVE_FLOAT);
    mtype1.insertMember(ASKP, HOFFSET(Quote, askp), PredType::NATIVE_FLOAT);
    mtype1.insertMember(ASKS, HOFFSET(Quote, asks), PredType::NATIVE_FLOAT);

    /*
     * Create the dataset.
     */
    DataSet* dataset;
    dataset = new DataSet(file->createDataSet(DATASET_NAME, mtype1, space));

    /*
     * Write data to the dataset;
     */
    dataset->write(quotes, mtype1);

    /*
     * Release resources
     */
    delete dataset;
    delete file;
  }  // end of try block

  // catch failure caused by the H5File operations
  catch(FileIException error) {
    error.printError();
    return -1;
  }

  // catch failure caused by the DataSet operations
  catch(DataSetIException error) {
    error.printError();
    return -1;
  }

  // catch failure caused by the DataSpace operations
  catch(DataSpaceIException error) {
    error.printError();
    return -1;
  }

  // catch failure caused by the DataSpace operations
  catch(DataTypeIException error) {
    error.printError();
    return -1;
  }
}

int readFromH5(vector <Quote> &result) {
  try {
    Exception::dontPrint();

    H5File* file = new H5File(FILE_NAME, H5F_ACC_RDONLY);

    CompType mtype1(sizeof(Quote));
    mtype1.insertMember(TSTAMP, HOFFSET(Quote, tstamp), PredType::NATIVE_FLOAT);
    mtype1.insertMember(NEMO, HOFFSET(Quote, nemo), PredType::NATIVE_INT);
    mtype1.insertMember(BIDP, HOFFSET(Quote, bidp), PredType::NATIVE_FLOAT);
    mtype1.insertMember(BIDS, HOFFSET(Quote, bids), PredType::NATIVE_FLOAT);
    mtype1.insertMember(ASKP, HOFFSET(Quote, askp), PredType::NATIVE_FLOAT);
    mtype1.insertMember(ASKS, HOFFSET(Quote, asks), PredType::NATIVE_FLOAT);

    DataSet* dataset;
    DataSpace* dataspace;
    hsize_t dims_out[1];
    cout << "Loading query data from files" << endl;
    dataset = new DataSet(file->openDataSet(DATASET_NAME));
    dataspace = new DataSpace(dataset->getSpace());

    int ndims = dataspace->getSimpleExtentDims(dims_out, NULL);
    int data_size = dims_out[0];

    Quote *quotes;
    quotes = new Quote[data_size];
    dataset->read(quotes, mtype1);
    cout << "Fetching Quotes" << endl;
    for (int i = 0; i < data_size; i++) {
       result.push_back(quotes[i]);
    }
    cout << "Cleaning objects" << endl;
    delete dataset;
    delete file;
  }

  catch(FileIException error) {
    error.printError();
    return -1;
  }

  catch(DataSetIException error) {
    error.printError();
    return -1;
  }

  catch(DataSpaceIException error) {
    error.printError();
    return -1;
  }

  catch(DataTypeIException error) {
    error.printError();
    return -1;
  }
}

  /* Generates filename accordin to the first quote recevev
   * this method assumes that all the quotes are aready coming
   * sorted by day
   */

string getFilename( vector <Quote> &quotes_v ){
    struct tm * ptm;
    double mtimestamp;
    char   buf[20];
    time_t rawtime;

    Quote* quotes =  &quotes_v[0];
    mtimestamp = quotes[0].tstamp;
    mtimestamp =  mtimestamp/(SID*1000);
    sprintf(buf,"%d.h5",(int) mtimestamp);
    return buf;
}

  /*
   * Extract from hdf5 the sorted data. Also apply a filter
   * and populates a vector of quotes.
   */

void ProcessResponse( Fxrequest request, vector <Quote> &result){

  long int begin_index;
  long int end_index;

  /* index is set accordin to the number of days from epoch.*/

  cout << "Procesing Response..." << endl << endl;

  begin_index =  request.begin_ts/(SID*1000);
  end_index = request.end_ts/(SID*1000);

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
      H5File* file = new H5File(FILE_NAME,
                                H5F_ACC_RDONLY );

      dataset = new DataSet( file->openDataSet(DATASET_NAME ));
      dataspace = new DataSpace (dataset->getSpace());

      int ndims = dataspace->getSimpleExtentDims( dims_out, NULL);
      int data_size = dims_out[0];

      Quote quotes[data_size];
      dataset->read( quotes, mtype1 );
      cout.precision(20);
      for( int j = 0; j < data_size; j++){
        if (((i == begin_index) and (quotes[j].tstamp >= request.begin_ts)) or
           ((i == end_index)   and (quotes[j].tstamp <= request.end_ts )) or
           ( i> begin_index and i < end_index )){
          result.push_back(quotes[j]);
        }
      }
      delete dataset;
      delete file;
    }catch( FileIException error ){
    }
  }
  cout << "Dispatch!";
}
