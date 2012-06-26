#include "../include/read_csv.h"
#include <vector>
#include <string>
#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <time.h>
#include <sstream>

#include <boost/tokenizer.hpp>
//#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;

int StrToInt(std::string const& s)
{
    std::istringstream iss(s);
    int value;
    if (!(iss >> value)) throw std::runtime_error("invalid int");
    return value;
}

//vector <string> vec;

vector < vector<string> > Readcsv(vector<string> files) {
  vector <string> vec;
  vector < vector<string> > csv;
  vector <int> vec_int;
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
      for(int i=2;i<vec.size();i++)  //filter
      {
        transform(vec.begin(), vec.end(), back_inserter(vec_int), StrToInt);
        vector<int>::const_iterator x = vec_int.begin();
        while(x!=vec_int.end()) 	 	   	
        {  
          if(!(*x>0)) break;
          x++;
        } 
      }
      csv.push_back(vec); 
    }
    //cout << csv.at(0).at(0) << '\n';
    y++;
  }
  return csv;
}
