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
using std::transform;
using std::back_inserter;
using std::ifstream;
using std::sort;
using std::ios;
using boost::bimap;
using boost::tokenizer;
using boost::regex;
using boost::escaped_list_separator;
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

vector<string> CsvHandler::readdir(string dir) {
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
          regex exp(NEMO_REGEXP);  //Symbols to find
          filename = (*it).string();
          if(regex_search(filename, exp))
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

  // map initialization
  bimap<string, int> nemo_map;
  nemo_map.insert(bimap<string, int>::value_type("EURUSD", 0));
  nemo_map.insert(bimap<string, int>::value_type("GBPUSD", 1));
  nemo_map.insert(bimap<string, int>::value_type("USDJPY", 2));

  while(y!=files.end()) {  //files cycle
    string data(*y);
    split(nemov,*y,is_any_of("bbo"));
    split(nemov,nemov.at(0),is_any_of("/")); 
    ifstream in(data.c_str(), ios::ate);
    if (!in.is_open()) cout << "file error" << endl;
    int itertok;
    long sizef;
    char *line;
    char *tokens;
    float tmp;
    string tokenstr;
    string linestr;
    string dates;
    string temp;
    
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
            dates = fixdate(dates, *tok_iter);
            quote.tstamp = totstamp(dates, *tok_iter);
          }
          else if(itertok==2){
            try{
              quote.bidp = lexical_cast<float>(*tok_iter);
            } catch(std::exception &excep_bidp){
                cout << "File  corrupted :" << endl;
                cout << *y << endl;
                cout << "linea: " << linestr << endl;
              }
          }
          else if(itertok==3){
	    try{
              quote.bids = lexical_cast<float>(*tok_iter);
            } catch(std::exception &excep_bids){
                cout << "File  corrupted :" << endl;
                cout << *y << endl;
                cout << "linea: " << linestr << endl;
              }    
          }
          else if (itertok==4){
            try{
              quote.askp = lexical_cast<float>(*tok_iter);
            } catch(std::exception excep_askp){
                cout << "File  corrupted :" << endl;
                cout << *y << endl;
                cout << "linea: " << linestr << endl;
              }
          }
          else if(itertok==5){
            try{
              quote.asks = lexical_cast<float>(*tok_iter);
            } catch(std::exception excep_asks){
                cout << "File  corrupted :" << endl;
                cout << *y << endl;
                cout << "linea: " << linestr << endl;
              }
          }
          itertok++;    
        }
	bimap<string, int>::left_const_iterator nemo_iterator;
      	nemo_iterator = nemo_map.left.find(nemov.back());
        quote.nemo = nemo_iterator->second;
        if(quote.bidp>0.0 && quote.bids>0.0 && quote.askp>0.0 && quote.asks>0.0 && quote.askp>=quote.bidp)
          quotes.push_back(quote);
        line = strtok(NULL, "\n");
        }
    }
    y++;
    in.close();
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
  //float splitfloat;
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
  return ms;
}

bool sortbytstamp( const Quote & lhs, const Quote & rhs )
{
   return lhs.tstamp < rhs.tstamp;
}

void CsvHandler::sortvec(vector <Quote> &vec) {
  sort(vec.begin(), vec.end(), sortbytstamp);  //vec sort 
}

 
