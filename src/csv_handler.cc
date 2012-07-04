#include "include/constants.h"
#include "include/csv_handler.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <boost/bimap.hpp>
#include <boost/regex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::istringstream;
using std::stringstream;
using boost::split;
using boost::is_any_of;
using std::ifstream;
using std::sort;
using std::ios;
using std::ios_base;
using boost::bimap;
using boost::tokenizer;
using boost::regex;
using boost::char_separator;
using boost::lexical_cast;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::filesystem;

float strtofloat(string const& s) {
  istringstream iss(s);
  float value;
  if (!(iss >> value)) throw std::runtime_error("invalid int");
  return value;
}

void CsvHandler::decompress(string dir){
  string comm = "cd data/raw && tar xfz ../../" + dir + " --strip=3"; 
  cout << "[Decompressing file] " << dir << endl;
  system(comm.c_str()); 
  return; 
}

vector<string> CsvHandler::readdir(string dir) {
  string filen, pathc, pathp;
  vector<string> archgz;
  vector<string> archcsv;
  pathp = dir+"/compressed";
  path p (pathp);
  try {
    if (exists(p)) {
      if (is_directory(p)){ 
        typedef vector<path> gz;
        gz gzpath;
        copy(directory_iterator(p), directory_iterator(), back_inserter(gzpath));
        sort(gzpath.begin(), gzpath.end());
        for (gz::const_iterator it(gzpath.begin()), it_end(gzpath.end()); it != it_end; ++it) {
          regex data("tar.gz");
          filen = (*it).string();
          if(regex_search(filen, data)){
              decompress(filen);
            }          
          }
          pathc = dir+"/raw";;
          path c (pathc);
          try {
            if(exists(c)) {
              if (is_directory(c)){
                typedef vector<path> csv;
                csv csvpath;
                copy(directory_iterator(c), directory_iterator(), back_inserter(csvpath));
                sort(csvpath.begin(), csvpath.end());
                vector<path>::iterator itercsv;                 
                for(itercsv = csvpath.begin(); itercsv < csvpath.end(); itercsv++){
                  regex exp(NEMO_REGEXP);
                  filen = (*itercsv).string();
                  if(regex_search(filen, exp))
                    archcsv.push_back(filen);
                }
                if(archcsv.empty()){
                  cout << "empty directory" << endl;
                  exit (1);
                } 
                return archcsv;
              }
            }
            else{
              cout << p << " does not exist" << endl;
              exit (1);
            }
          } 
          catch (const filesystem_error& ex) {
            cout << ex.what() << endl;
          }
      }else{
         cout << p << " exists, but is neither a regular file nor a directory" << endl;
         exit (1);
       }
     }
      else{
          cout << p << " does not exist" << endl;
          exit (1);
        }
      }
      catch (const filesystem_error& ex) {
        cout << ex.what() << endl;
      }
}
	
vector <Quote> CsvHandler:: readcsv(vector<string> files) {
  // structure vector and structure initialization
  vector <Quote> quotes;
  Quote quote;
  vector <string> nemov;
  vector<string>::const_iterator array_files = files.begin();

  // map initialization
  bimap<string, int> nemo_map;
  nemo_map.insert(bimap<string, int>::value_type("EURUSD", 0));
  nemo_map.insert(bimap<string, int>::value_type("GBPUSD", 1));
  nemo_map.insert(bimap<string, int>::value_type("USDJPY", 2));

  //files cycle
  while(array_files!=files.end()) {  
    string data(*array_files);
    cout << "[Loading CSV] " << data<< endl;
    split(nemov,*array_files,is_any_of("bbo"));
    split(nemov,nemov.at(0),is_any_of("/")); 
    ifstream in(data.c_str(), ios::ate);
    if (!in.is_open()) cout << "file error" << endl;
    int itertok;
    long sizef;
    char *line;
    string linestr, dates, hrs;
    sizef = in.tellg();
    in.seekg(0, ios::beg);
    line = new char[sizef];
    while(!in.eof()){
      in.read(line,sizef+1);
      line = strtok(line, "\n");
      while(line!=NULL){
        linestr = line;
        typedef tokenizer < char_separator <char> > tokenizer;
        char_separator<char> sep(",");
        tokenizer tokens(linestr, sep);
        itertok = 0;
        for (tokenizer::iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter){
          if(itertok==0)
            dates = *tok_iter;
          else if(itertok==1){
            hrs = *tok_iter;
          }
          else if(itertok==2){
            try{
              quote.bidp = lexical_cast<float>(*tok_iter);
            } catch(std::exception &excep_bidp){
                cout << "Line corrupted on file :" << endl;
                cout << "   " << *array_files << endl;
                cout << "    value: " << linestr << endl;
              }
          }
          else if(itertok==3){
	    try{
              quote.bids = lexical_cast<float>(*tok_iter);
            } catch(std::exception &excep_bids){
                cout << "Line corrupted on file :" << endl;
                cout << "   " << *array_files << endl;
                cout << "    value: " << linestr << endl;
              }    
          }
          else if (itertok==4){
            try{
              quote.askp = lexical_cast<float>(*tok_iter);
            } catch(std::exception excep_askp){                
                cout << "Line corrupted on file :" << endl;
                cout << "   " << *array_files << endl;
                cout << "    value: " << linestr << endl; 
              }
          }
          else if(itertok==5){
            try{
              quote.asks = lexical_cast<float>(*tok_iter);
            } catch(std::exception excep_asks){
                cout << "Line corrupted on file :" << endl;
                cout << "   " << *array_files << endl;
                cout << "    value: " << linestr << endl; 
              }
          }
          itertok++;    
        }
	if(quote.bidp>0.0 && quote.bids>0.0 && quote.askp>0.0 && quote.asks>0.0 && quote.askp>=quote.bidp){
          bimap<string, int>::left_const_iterator nemo_iterator;
      	  nemo_iterator = nemo_map.left.find(nemov.back());
          quote.nemo = nemo_iterator->second;
          dates = fixdate(dates, hrs);
          quote.tstamp = totstamp(dates, hrs);
          quotes.push_back(quote);
        }
        line = strtok(NULL, "\n");
        }
    }
    array_files++;
    in.close();
  }
  nemov.clear();
  return quotes;
}

string CsvHandler::fixdate(string fdate, string hrs) {
  int dayint;
  string stringdate;
  stringstream day(hrs);
  day >> dayint;
  date daydate(from_simple_string(fdate));
  if(dayint>=17){        
    date_duration dd(1);
    daydate = daydate - dd;
  }  
  stringdate = to_iso_extended_string(daydate);
  return stringdate;  	
}

double CsvHandler::totstamp(string fdate, string hrs) {
  int tstamp, dayint;
  double timestampfloat, timestamp, splitfloat;
  string tstamps, stringdate;
  vector <string> splitstring;

  split(splitstring,hrs,is_any_of("."));  
  stringstream splits(splitstring.at(1));
  splits >> splitfloat; 
  tstamps = ""+fdate+" "+hrs+"";
  ptime tstampd = (time_from_string(tstamps));
  ptime timet_start(date(1970,1,1));
  time_duration const diff = tstampd - timet_start;
  long long ms = diff.total_seconds();
  ms *= 1000;
  ms += splitfloat;
  splitstring.clear();
  return ms;
}

bool sortbytstamp( const Quote & lhs, const Quote & rhs )
{
   return lhs.tstamp < rhs.tstamp;
}

void CsvHandler::sortvec(vector <Quote> &vec) {
  sort(vec.begin(), vec.end(), sortbytstamp);  //vec sort 
}

 
