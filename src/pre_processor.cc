#include "read_csv.cc"
#include "read_dir.cc"
#include "sort_vec.cc"
#include <vector>
#include <string>
#include <time.h>
#include <iostream>

int main(){
  
  clock_t tStart = clock();
  std::vector<string> files;
  std::vector < vector<string> > gline;
  std::vector < vector<string> > sorted; 
  files = Readdir();
  cout << "Reading dir time:" <<  (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  gline = Readcsv(files);
  cout << "Reading csv time:" <<  (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  /*for(int j=0;j<gline.size();j++)
  {
    for(int i=0;i<gline.at(0).size();i++)
      cout << gline.at(j).at(i) << '|';
    cout << endl;
  }*/
  //cout << endl;
  sorted = Sortvec(gline);
  cout << "Sort time:" <<  (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
  /*for(int j=0;j<sorted.size();j++)
  {
    for(int i=0;i<sorted.at(0).size();i++)
      cout << sorted.at(j).at(i) << '|';
      cout << endl;
  }*/ 
  return 0;
}
      
