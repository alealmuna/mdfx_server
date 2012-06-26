#include "Readcsv.h"
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

vector<string> Readcsv(char *file) {
  vector <string> vec;
  vector <int> vec_int;
  string data(file);
  ifstream in(data.c_str());
  if (!in.is_open()) return vec;
  typedef tokenizer< escaped_list_separator<char> > Tokenizer;
  string line;
  while (getline(in,line))
  {
    Tokenizer tok(line);
    vec.assign(tok.begin(),tok.end());
    if (!(vec.size() < 3)) return vec;
    for(int i=2;i<vec.size();i++)
    {
      transform(vec.begin(), vec.end(), back_inserter(vec_int), StrToInt);
      vector<int>::const_iterator x = vec_int.begin();
      while(x!=vec_int.end())	 	   	
      {  
        if(!(*x>0)) return vec;
        x++;
      } 
    } 
    copy(vec.begin(), vec.end(),
    ostream_iterator<string>(cout, "|")); 
    cout << "\n----------------------" << endl;
    //cout << vec.size() << '\n';
    return vec;
  }
}
