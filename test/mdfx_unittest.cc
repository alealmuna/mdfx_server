#include <limits.h>
#include <fstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"
//#include "include/proto_handler.h"
#include "include/csv_handler.h"
//#include "test/interfaces.pb.h"

using std::fstream;
using std::ios;
using std::vector;
using std::string;

/*
class FXRequestTest :  public ::testing::Test{
  protected:
  ProtoHandler phandler;
  mdfx_server::FXRequest request_in;
  mdfx_server::FXRequest request_out;
};
*/
class CsvHandlerTest : public ::testing::Test{
  protected:
  CsvHandler chandler;
};
/*
TEST_F(FXRequestTest,ReadProtobuf){
  fstream output("test/test_request", ios::out | ios::trunc | ios::binary);
  
  request_out.set_begin_timestamp(1309469186330);
  request_out.set_end_timestamp(1309469193440);
  request_out.set_max_rel_spread(0.01);
  
  request_out.add_nemo_list("EURUSD");
  request_out.add_nemo_list("USDGBP");
  request_out.SerializeToOstream(&output);
  string filename("test/test_request");
  EXPECT_TRUE(phandler.readRequestFromFile(filename));
  remove(filename.c_str());
  google::protobuf::ShutdownProtobufLibrary();
}
*/

TEST_F(CsvHandlerTest, Readdir){
  string dir = "test/data";
  vector <string> files;
  vector <string> files_rd;
  files.push_back("test/data/raw/EURUSDbbotest.csv");
  files.push_back("test/data/raw/USDJPYbbotest.csv");
  files_rd = chandler.readdir(dir); 
  EXPECT_EQ(files.at(0), files_rd.at(0));
  EXPECT_EQ(files.at(1), files_rd.at(1));
  files.clear();
  files_rd.clear();
}

TEST_F(CsvHandlerTest,Readcsv){
  vector <string> files;
  vector <Quote> testvec;
  files.push_back("test/data/raw/EURUSDbbotest.csv");
  files.push_back("test/data/raw/USDJPYbbotest.csv");
  //files.push_back("../staging/storrealba/cart_storrealba_20120615_095234010/EURUSDbbo_201101010000_201102282359.csv");
  testvec = chandler.readcsv(files);
  //first file vector
  EXPECT_EQ((long long)1293987739870, (long long)testvec.at(0).tstamp);
  EXPECT_EQ(0, testvec.at(0).nemo);
  EXPECT_EQ((float)1.33182, (float)testvec.at(0).bidp);
  EXPECT_EQ(1500000, testvec.at(0).bids);
  EXPECT_EQ((float)12.4, (float)testvec.at(0).askp); 
  EXPECT_EQ(1, testvec.at(0).asks);
  //second file vector   
  EXPECT_EQ((long long)1293987739870, (long long)testvec.at(14).tstamp);
  EXPECT_EQ(2, testvec.at(14).nemo);
  EXPECT_EQ((float)1.33182, (float)testvec.at(14).bidp);
  EXPECT_EQ(1500000, testvec.at(14).bids);
  EXPECT_EQ((float)12.4, (float)testvec.at(14).askp); 
  EXPECT_EQ(1, testvec.at(14).asks);
  files.clear();
  testvec.clear();  
}

TEST_F(CsvHandlerTest,Fixdate){
  string fdate = "2011/01/03";
  string hrs = "17:05:17.562";
  EXPECT_EQ("2011-01-02", chandler.fixdate(fdate, hrs));
}

TEST_F(CsvHandlerTest,Tostamp){
  string fdate = "2011-01-02";
  string hrs = "17:05:17.562";
  double tstamporg = 1293987917562;
  double tstampnew = chandler.totstamp(fdate,hrs);
  EXPECT_EQ((long)tstamporg, (long)tstampnew);
}

TEST_F(CsvHandlerTest,Sortvec) {
  //vector <string> files;
  //vector <Quote> testvec;
  //files.push_back("test/data/raw/EURUSDbbotest.csv");
  //files.push_back("test/data/raw/USDJPYbbotest.csv");
  //testvec = chandler.readcsv(files);
  
}

