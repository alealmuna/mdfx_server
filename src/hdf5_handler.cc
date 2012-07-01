#include <string>
#include <vector>
#include <iostream>
#include <time.h>
#include <math.h>

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
#include "include/constants.h"

#ifndef H5_NO_NAMESPACE
    using namespace H5;
#endif

  /*
   * Coverts a milisecond timestamp to
   * the number of the days since 1900.
   */

double dayFromEpoch(double tstamp){
  double day;
  day =  int (tstamp/(SID*1000));
  return day;
}

  /*
   *  Separate the quote vector in small vectors
   *  per day.
   */

int writeToH5perDay(vector <Quote> &quotes_v){
  double last_date;
  double quote_date;
  int status = 0;
  vector <Quote> dailyq;
  cout << "Write per day" << endl;
  last_date = dayFromEpoch(quotes_v[0].tstamp);
  while(0<quotes_v.size()) {
    quote_date = dayFromEpoch(quotes_v[0].tstamp);
    cout.precision(30);
    if (quote_date == last_date){
      /* last quote insert the remain vector */
      dailyq.push_back(quotes_v[0]);
      if (quotes_v.size()==1){
        cout << get_filename(dailyq) << endl;
        status =  writeToH5(dailyq ,get_filename(dailyq));
        dailyq.clear();
      }
    }else{
      status = writeToH5(dailyq ,get_filename(dailyq));
      last_date = quote_date;
      dailyq.clear();
      dailyq.push_back(quotes_v[0]);
    };
    quotes_v.erase(quotes_v.begin());
  };
}

int writeToH5(vector <Quote> &quotes_v, string filename) {
  try {
    int length = quotes_v.size();
    Quote* quotes = &quotes_v[0];
    Exception::dontPrint();
    H5std_string FILENAME(filename);
    /*
     * Create the data space.
     */
    hsize_t dim[] = {length};   /* Dataspace dimensions */
    DataSpace space(RANK, dim);
    /*
     * Create the file.
     */
    H5File* file = new H5File(FILENAME, H5F_ACC_TRUNC);
    
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

string get_filename( vector <Quote> &quotes_v ){
    struct tm * ptm;
    double mtimestamp;
    char   buf[40];
    time_t rawtime;
    string datapath = "data/";

    Quote* quotes =  &quotes_v[0];
    mtimestamp = quotes[0].tstamp;
    mtimestamp =  mtimestamp/(SID*1000);
    sprintf(buf,"%d.h5",(int) mtimestamp);
    datapath += buf;
    return datapath;
}

  /*
   * Check if the instrument belogs to the 
   * provided list.
   */

bool nemo_in(/*vector <int>*/int * nemo_vct, int value){
  for (int i = 0; i < /*nemo_vct.size()*/ 3 ;i++){
    if (nemo_vct[i] == value){
      return true;
    }
  }
  return false;
}
  /*
   * Generates a file_path based on a double.
   */
string format_filename(int index){
    char buf[40];
//    string datapath = "/home/oscar/src/mdfx_server/data/";
    string datapath = "data/";
    sprintf(buf,"%d.h5", index);
    datapath += buf;
    return datapath;
}

  /*
   * Evaluates if a quote is according
   * to the restrictions.
   */

bool is_valid_q(Quote &quote, double mrs){
  bool mrs_flag = quote.askp/quote.bidp >= exp(1 + mrs);
  if ((quote.bidp <= 0) or (quote.askp <= 0) or
     (quote.bids <= 0) or (quote.asks <= 0) or
     (quote.bidp > quote.askp) or mrs_flag)
      return false;
  return true;
}

  /*
   * Evaluates if a quote is valid according to
   * the date range required.
   */

bool is_candidate(Quote &quote, Fxrequest request,
                  long int bgn_indx, long int end_indx,
                  int current){

  bool first_doc;
  bool last_doc;
  bool qu_over;
  bool qu_under;
  bool qu_between;

  first_doc = (current == bgn_indx);
  last_doc = ( current == end_indx);
  /* request timestamp evaluated in miliseconds */
  qu_over = (quote.tstamp >= request.begin_ts*1000);
  qu_under = (quote.tstamp <= request.end_ts*1000 );
  qu_between = ( current > bgn_indx and current < end_indx );

  cout.precision(20);

  if ((first_doc and qu_over) or
     (last_doc and qu_under) or
     (qu_between)){
    return nemo_in(request.nemo,quote.nemo);
  };
  return false;
}
  
  /*
   * Extract from hdf5 the sorted data. Also apply a filter
   * and populates a vector of quotes.
   */

void ProcessResponse( Fxrequest request, vector <Quote> &result){

  long int bgn_indx;
  long int end_indx;

  /* index is set accordin to the number of days from epoch.*/

  cout << "Procesing Response..." << endl << endl;

  bgn_indx =  request.begin_ts/(SID);
  end_indx = request.end_ts/(SID);

  cout << "Begin Index:" << bgn_indx << endl;
  cout << "End Index: " << end_indx << endl;

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

  for (int i = bgn_indx; i <= end_indx; i++){
    try{
      H5std_string FILE_NAME(format_filename(i));
      H5File* file = new H5File(FILE_NAME,
                                H5F_ACC_RDONLY );

      dataset = new DataSet( file->openDataSet(DATASET_NAME ));
      dataspace = new DataSpace (dataset->getSpace());

      int ndims = dataspace->getSimpleExtentDims( dims_out, NULL);
      int data_size = dims_out[0];

      Quote quotes[data_size];
      dataset->read( quotes, mtype1 );

      for( int j = 0; j < data_size; j++){
        if (is_candidate(quotes[j], request, bgn_indx, end_indx, i)){
//             is_valid_q(quotes[j],request.max_rel_spread)){
                  result.push_back(quotes[j]);
        };
      };
      delete dataset;
      delete file;
    }catch( FileIException error ){
    }
  }
  cout << "Dispatch!" << endl << endl;
}

