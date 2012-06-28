#include "../include/read_dir.h"
#include <iostream>     
#include <fstream>      
#include <vector>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

vector<string> Readdir()
{
  using namespace std;
  using namespace boost;
  using namespace boost::filesystem;

  string simb[] = {"EURUSD", "USDGBP", "JPYUSD"}, filename;
  vector<string> arch;
  path p ("../../staging/storrealba/cart_storrealba_20120615_095234010/");
  try
  {
    if (exists(p))
    {
      if (is_regular_file(p))
        cout << p << " size is " << file_size(p) << endl;

      else if (is_directory(p))
      {
        typedef vector<path> vec;
        vec v;

        copy(directory_iterator(p), directory_iterator(), back_inserter(v));
        sort(v.begin(), v.end());

        for (vec::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it)
        {
	  regex e("(USDSGD).*");  //Simbols to find
	  filename = (it->filename()).string();
	  if(regex_match(filename, e))
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
  catch (const filesystem_error& ex)
  {
    cout << ex.what() << endl;
  }

  return arch;
}	

