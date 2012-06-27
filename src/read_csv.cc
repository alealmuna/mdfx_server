#include "../include/read_csv.h"
#include <vector>
#include <string>
#include <iostream>     
#include <fstream>      
#include <algorithm>    
#include <iterator>     
#include <time.h>
#include <sstream>

#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

float StrToInt(std::string const& s)
{
    std::istringstream iss(s);
    float value;
    if (!(iss >> value)) throw std::runtime_error("invalid float");
    return value;
}

vector < vector<string> > Readcsv(vector<string> files) {
  vector <string> vec;
  vector < vector<string> > csv;
  vector <float> vec_int;
  vector<string>::const_iterator y = files.begin();
  while(y!=files.end())  //files cycle		
  {
    string data("../../staging/storrealba/cart_storrealba_20120615_095234010/"+*y);
    ifstream in(data.c_str());
    if (!in.is_open()) return csv;
    typedef tokenizer< escaped_list_separator<char> > Tokenizer;
    string line;
    while (getline(in,line))  //get quote
    {
      Tokenizer tok(line);
      vec.assign(tok.begin(),tok.end());
      transform(vec.begin(), vec.end(), back_inserter(vec_int), StrToInt);  //vector to float for filter operations
      vector<float>::const_iterator x = vec_int.begin()+2;
      bool valid = true;
      while(x!=vec_int.end())   	 	   	
      {  
        if(!(*x>0)) valid=false;  //filter >0
          x++;
      }
      if(!(vec_int.at(4)>=vec_int.at(2))) valid=false;  //filter ask_price >= bid_price 
      if(valid==true) csv.push_back(vec);
      vec_int.clear(); 
    }
    y++;  
  }
  return csv;
}
