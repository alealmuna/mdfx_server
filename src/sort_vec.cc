#include "../include/sort_vec.h"
#include <vector>
#include <string>
#include <sstream>
#include <boost/regex.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

vector < vector<string> > Sortvec (vector < vector<string> > vec)
{
  vector < vector<string> > sorted;
  vector <string> datesplit;
  vector <string> timestamp;
  string stringdate, stringsort;
  int dayint;
  for(int i=0;i<vec.size();i++)
  {
    stringstream day(vec.at(i).at(1));
    day >> dayint;
    if(dayint>=17)  //date fix
    {      
      date daydate(from_simple_string(vec.at(i).at(0)));
      date_duration dd(1);
      daydate = daydate - dd;
      stringdate = to_iso_extended_string(daydate);
      split(datesplit, stringdate, boost::is_any_of("-"));
      vec.at(i).at(0) = ""+datesplit.at(0)+"/"+datesplit.at(1)+"/"+datesplit.at(2)+"";	
    }
  }
  sort (vec.rbegin(), vec.rend());  //vec sort   
  return vec;
}
