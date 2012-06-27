#include <string> 
#include <vector>
#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <sstream>

#define FILE            "myfile.h5"
#define DATASET         "DS1"
#define DIM0     8                 /* size of dataset */       
#define DIM1     10 

using namespace std;

/* Converts from vector<string> to array of chars */
char **str_to_char(vector<string> & hold){
  char  **arr = new char*[hold.size()];
  for(size_t i = 0; i < hold.size(); i++){
    arr[i] = new char[hold[i].size() + 1];
    strcpy(arr[i], hold[i].c_str());
  }
  return arr;
}

/* Dummy vector population */
vector < vector<string> >populate(){
  vector < vector<string> > hold;
  stringstream out;
  int         ndims,i;
  for (int i = 0; i < 3; i++) {
    vector<string> vec;
    out << i;
    vec.push_back("2011/01/03/"+out.str());
    vec.push_back("17:05:11.583");                                                 
    vec.push_back("1,01345");
    vec.push_back("1000");
    vec.push_back("0");
    vec.push_back(out.str());
    hold.push_back(vec);
  }
  return hold;
}

int main (void)
{
    hid_t       file, filetype, memtype, space, dset;
    herr_t      status;
    hsize_t     dims[2];
    vector      < vector<string> >csv;
    int         ndims,i;
    const char  *data[3][6];
    csv = populate();

    /*
     * Create a new file using the default properties.
     */
    file = H5Fcreate (FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    /*
     * Create file and memory datatypes.  For this example we will save
     * the strings as FORTRAN strings.
     */
    filetype = H5Tcopy (H5T_FORTRAN_S1);
    status = H5Tset_size (filetype, H5T_VARIABLE);
    memtype = H5Tcopy (H5T_C_S1);
    status = H5Tset_size (memtype, H5T_VARIABLE);
    /*
     * Create dataspace.  Setting maximum size to NULL sets the maximum
     * size to be the current size.
     */
    dims[0] = 3;
    dims[1] = 6;
    space = H5Screate_simple (2, dims, NULL);

    /*
     * Create the dataset and write the variable-length string data to
     * it.
     */
    dset = H5Dcreate (file, DATASET, filetype, space, H5P_DEFAULT, H5P_DEFAULT,
                H5P_DEFAULT);

    for(size_t i = 0; i < csv.size(); i++)
      for(size_t j = 0; j < csv[i].size(); j++)
        data[i][j] = csv[i][j].c_str();

    status = H5Dwrite (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    /*
     * Close and release resources.
     */
    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Tclose (filetype);
    status = H5Tclose (memtype);
    status = H5Fclose (file);

    return 0;
}
