#include "include/csv_handler.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <boost/regex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

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
using namespace boost::gregorian;
using namespace boost::filesystem;
using boost::tokenizer;
using boost::regex;
using boost::escaped_list_separator;

float strtofloat(string const& s) {
  istringstream iss(s);
  float value;
  if (!(iss >> value)) throw std::runtime_error("invalid int");
  return value;
}

vector<string> CsvHandler::Readdir() {
  string simb[] = {"EURUSD", "USDGBP", "JPYUSD"}, filename;
  vector<string> arch;
  path p ("../../staging/storrealba/cart_storrealba_20120615_095234010/");
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
	  regex exp("(USDSGD).*");  //Simbols to find
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
	
vector < vector<string> > CsvHandler:: Readcsv(vector<string> files) {
  vector <string> vec;
  vector < vector<string> > csv;
  vector <float> vec_float;
  vector<string>::const_iterator y = files.begin();
  while(y!=files.end()) {  //files cycle
    string data("../../staging/storrealba/cart_storrealba_20120615_095234010/"+*y);
    ifstream in(data.c_str());
    if (!in.is_open()) return csv;
    typedef tokenizer< escaped_list_separator<char> > Tokenizer;
    string line;
    while (getline(in,line)) {
      Tokenizer tok(line);
      vec.assign(tok.begin(),tok.end());
      transform(vec.begin(), vec.end(), back_inserter(vec_float),strtofloat);  //vector to float for filter operations
      vector<float>::const_iterator itervec = vec_float.begin()+2;
      bool valid = true;
      while(itervec!=vec_float.end()) {  
        if(!(*itervec>0)) valid=false;  //filter >0
          itervec++;
      }
      if(!(vec_float.at(4)>=vec_float.at(2))) valid=false;  //filter ask_price >= bid_price 
      if(valid==true) csv.push_back(vec);
      vec_float.clear(); 
    }
    y++;  
  }
  return csv;
}

vector < vector<string> > CsvHandler::Sortvec(vector < vector<string> > vec) {
  vector < vector<string> > sorted;
  vector <string> datesplit;
  vector <string> timestamp;
  string stringdate, stringsort;
  int dayint;
  for(int i=0;i<vec.size();i++)
  {
    stringstream day(vec.at(i).at(1));
    day >> dayint;
    if(dayint>=17)  {  //date fix      
      date daydate(from_simple_string(vec.at(i).at(0)));
      date_duration dd(1);
      daydate = daydate - dd;
      stringdate = to_iso_extended_string(daydate);
      split(datesplit, stringdate, is_any_of("-"));
      vec.at(i).at(0) = ""+datesplit.at(0)+"/"+datesplit.at(1)+"/"+datesplit.at(2)+"";	
    }
  }
  sort (vec.rbegin(), vec.rend());  //vec sort   
  return vec;
}

 
