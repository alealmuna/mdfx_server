#include "read_csv.cc"
#include "read_dir.cc"
#include <vector>
#include <string>
#include <iostream>

int main(){
  
  std::vector<string> files;
  std::vector< vector<string> > gline;
  files = Readdir();
  gline = Readcsv(files);
  for(int i=0;i<gline.at(0).size();i++)
    cout << gline.at(0).at(i) << '|';
    cout << '\n';   
  return 0;
}
      
