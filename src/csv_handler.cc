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

void CsvHandler::decompress_command(string dir){
  string path_decom, file_n;
  
  path_decom = dir+"/compressed";
  path decom (path_decom);
 
  /* read tar.gz directory */ 
  try{
    if (exists(decom)){
      if (is_directory(decom)){
        typedef vector<path> gz;
        gz gzpath;
        copy(directory_iterator(decom), directory_iterator(), back_inserter(gzpath));
        sort(gzpath.begin(), gzpath.end());
        for (gz::const_iterator it(gzpath.begin()), it_end(gzpath.end()); it != it_end; ++it) {
          regex data("tar.gz");
          file_n = (*it).string();
          if(regex_search(file_n, data))
            decompress(file_n);                    
        }
      } else 
          cout << decom << " exists, but not a directory" << endl;
    } else
        cout << decom << " does not exist" << endl; 
  } catch (const filesystem_error& ex){
      cout << ex.what() << endl;
  }  
}

void CsvHandler::decompress(string dir){
  string comm = "cd data/raw && tar xfz ../../" + dir + " --strip=3"; 
  cout << "[Decompressing file] " << dir << endl;
  system(comm.c_str());
  comm.clear(); 
}

/* Directory reader */

vector<string> CsvHandler::readdir(string dir) {
  string file_n, path_csv;
  vector<string> file_csv;
  
  /* decompress tar.gz */ 
  decompress_command(dir);

  /* read csv directory */
  path_csv = dir+"/raw";;
  path p_csv (path_csv);
  try {
    if(exists(p_csv)) {
      if (is_directory(p_csv)){
        typedef vector<path> csv;
        csv csv_path;
        copy(directory_iterator(p_csv), directory_iterator(), back_inserter(csv_path));
        sort(csv_path.begin(), csv_path.end());
        vector<path>::iterator iter_csv;
                         
        for(iter_csv = csv_path.begin(); iter_csv < csv_path.end(); iter_csv++){
          regex exp(NEMO_REGEXP);
          file_n = (*iter_csv).string();
          if(regex_search(file_n, exp))
            file_csv.push_back(file_n);
        }
        
        if(file_csv.empty())
          cout << "empty directory" << endl;

        return file_csv;

      } else 
          cout << p_csv << " exists, but is neither a regular file nor a directory" << endl;
    } else 
        cout << p_csv << " does not exist" << endl;
  } catch (const filesystem_error& ex){
      cout << ex.what() << endl;      
  }
}

/* csv reader */
	
vector <Quote> CsvHandler::readcsv(vector<string> files) {
  // structure vector and structure initialization
  vector <Quote> quotes;
  Quote quote;
  vector<string>::const_iterator array_files = files.begin();

  //files cycle
  while (array_files != files.end()) {  
    string data(*array_files);
    vector <string> nemo;
    int itertok;
    long size_file;
    char *line;
    string linestr, dates, hrs;

    cout << "[Loading CSV] " << data << endl;
    /* Get nemo from file name */
    split(nemo, *array_files,is_any_of("bbo"));
    split(nemo, nemo.at(0),is_any_of("/"));
    /* Open csv file */ 
    ifstream in(data.c_str(), ios::ate);
    if (!in.is_open()) cout << "file error" << endl;
    size_file = in.tellg();
    in.seekg(0, ios::beg);
    line = new char[size_file];

    /* Read csv file */
    while (!in.eof()) {
      in.read(line, size_file+1);
      line = strtok(line, "\n");

      /* Split by line */
      while(line != NULL){
        typedef tokenizer < char_separator <char> > tokenizer;
        char_separator<char> sep(",");
        linestr = line;
        tokenizer tokens(linestr, sep);
        itertok = 0;
        try {
          /* Split by element */
          for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
            if (itertok == 0)
              dates = *tok_iter;
            else if (itertok == 1)
              hrs = *tok_iter;
            else if (itertok == 2)
              quote.bidp = lexical_cast<float>(*tok_iter);
            else if (itertok == 3)
              quote.bids = lexical_cast<float>(*tok_iter);
            else if (itertok == 4)
              quote.askp = lexical_cast<float>(*tok_iter);
            else if (itertok == 5)
              quote.asks = lexical_cast<float>(*tok_iter);
            itertok++;    
          }
        } catch (std::exception exec_cast) {
          cout << "Line corrupted on file :" << endl;
          cout << "   " << *array_files << endl;
          cout << "    value: " << linestr << endl; 
        }
        /* Filters */
	if (quote.bidp > 0.0 && quote.bids > 0.0 && quote.askp > 0.0 && quote.asks > 0.0 && quote.askp >= quote.bidp) {
          bimap<string, int>::left_const_iterator nemo_iterator;
      	  nemo_iterator = nemo_map.left.find(nemo.back());
          quote.nemo = nemo_iterator->second;
          /* day fix */
          dates = fixdate(dates, hrs);
          /* date to tstamp */
          quote.tstamp = totstamp(dates, hrs);
          /* save quote in vector */
          quotes.push_back(quote);
        }
        line = strtok(NULL, "\n");
      }
    }
    array_files++;

    delete[] line;
    data.clear();
    linestr.clear();
    dates.clear();
    hrs.clear();
    nemo.clear();
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
  int dayint;
  double timestampfloat, timestamp, splitfloat;
  string tstamp;
  vector <string> splitstring;

  split(splitstring, hrs, is_any_of("."));  
  stringstream splits(splitstring.at(1));
  splits >> splitfloat; 
  tstamp = ""+fdate+" "+hrs+"";
  ptime tstampd = (time_from_string(tstamp));
  ptime timet_start(date(1970,1,1));
  time_duration const diff = tstampd - timet_start;
  long long ms = diff.total_seconds();
  ms *= 1000;
  ms += splitfloat;
  
  splitstring.clear();
  tstamp.clear();

  return ms;
}

/* quotes sort */

bool sortbytstamp( const Quote & lhs, const Quote & rhs )
{
   return lhs.tstamp < rhs.tstamp;
}

void CsvHandler::sortvec(vector <Quote> &vec) {
  sort(vec.begin(), vec.end(), sortbytstamp);  //vec sort 
}

 
