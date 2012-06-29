#include "include/constants.h"
#include "include/csv_handler.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <boost/regex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::istringstream;
using std::stringstream;
using boost::split;
using boost::is_any_of;
using std::transform;
using std::back_inserter;
using std::ifstream;
using std::map;
using std::sort;
using boost::tokenizer;
using boost::regex;
using boost::escaped_list_separator;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::filesystem;

float strtofloat(string const& s) {
  istringstream iss(s);
  float value;
  if (!(iss >> value)) throw std::runtime_error("invalid int");
  return value;
}

vector<string> CsvHandler::readdir(char * dir) {
  string filename;
  vector<string> arch;
  path p (dir);
  try {
    if (exists(p)) {
      if (is_regular_file(p))
        cout << p << " size is " << file_size(p) << endl;
      else if (is_directory(p)) {
        typedef vector<path> vec;
        vec v;
        copy(directory_iterator(p), directory_iterator(), back_inserter(v));
        sort(v.begin(), v.end());
        for (vec::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it) {
	  regex exp("(EURUSD).*");  //Simbols to find
	  filename = (it->filename()).string();
	  if(regex_match(filename, exp))
	    arch.push_back(filename);
        }
	return arch; // csv directory ls
      }
      else
        cout << p << " exists, but is neither a regular file nor a directory" << endl;
    }
    else
      cout << p << " does not exist" << endl;
  }
  catch (const filesystem_error& ex) {
    cout << ex.what() << endl;
  }
  return arch;
}
	
vector <Quote> CsvHandler:: readcsv(vector<string> files) {
  int iter=0;
  vector <Quote> quotes;
  Quote quote;
  vector <string> vec;
  vector <string> nemov;
  vector <string> nemoff;
  vector < vector<string> > csv;
  vector <float> vec_float;
  vector<string>::const_iterator y = files.begin();
  map<string, int> nemo_map;

  // map initialization
  nemo_map["EURUSD"] = 1;
  nemo_map["GBPUSD"] = 2;
  nemo_map["USDJPY"] = 3;

  while(y!=files.end()) {  //files cycle
    string data("test/dirtest/"+*y);
    split(nemov,*y,is_any_of("bbo")); 
    ifstream in(data.c_str());
    if (!in.is_open()) return quotes;
    typedef tokenizer< escaped_list_separator<char> > Tokenizer;
    string line;
    while (getline(in,line)) {
      Tokenizer tok(line);
      vec.assign(tok.begin(),tok.end());
      vec.at(0) = fixdate(vec.at(0), vec.at(1));
      istringstream bidpf(vec.at(2));
      istringstream bidsf(vec.at(3));
      istringstream askpf(vec.at(4));
      istringstream asksf(vec.at(5));
      bidpf >> quote.bidp;
      bidsf >> quote.bids;
      askpf >> quote.askp;
      asksf >> quote.asks;
      quote.nemo = nemo_map[nemov.at(0)];
      if(quote.bidp>0.0 && quote.bids>0 && quote.askp>0.0 && quote.asks>0 && quote.askp>=quote.bidp) {
        quotes.push_back(quote);
      }       
    }
    y++;  
  }
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
  float splitfloat;
  double timestampfloat, timestamp;
  string tstamps, stringdate;
  vector <string> splitstring;

  split(splitstring,hrs,is_any_of("."));  
  stringstream splits(splitstring.at(1));
  splits >> splitfloat; 
  splitfloat /=1000;  
  tstamps = ""+fdate+" "+hrs+"";
  date tstampd (from_simple_string(tstamps));
  cout << tstampd << endl;
  ptime timet_current(tstampd);
  ptime timet_start(date(1970,1,1));
  time_duration diff = timet_current - timet_start;
  timestamp = diff.ticks()/time_duration::rep_type::ticks_per_second;
  timestamp += splitfloat;
  timestamp *=1000;
  return timestamp;
}

bool sortbytstamp( const Quote & lhs, const Quote & rhs )
{
   return lhs.tstamp < rhs.tstamp;
}

void CsvHandler::sortvec(vector <Quote> &vec) {
  sort(vec.begin(), vec.end(), sortbytstamp);  //vec sort 
}

 
