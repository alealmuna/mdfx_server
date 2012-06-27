#include "hdf5.h"
#define FILE "groups.h5"
#include <string>
#include <vector>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

int main() {

   hid_t       file_id, year_group_id,month_group_id, day_group_id, dataset_id, datatype,dataspace_id;  /* identifiers */
   hsize_t     dims[2];
   herr_t      status;
   string      name;
   char        **rdata;
   string      date[3];
   vector<string> hold;
   char_separator<char> sep("/");

   hold.push_back("2011/01/03");
   hold.push_back("17:05:11.583");
   hold.push_back("1,01345");
   hold.push_back("1000000");
   hold.push_back("0");
   hold.push_back("0");

   for (int i = 0; i < hold.size(); i++)
     cout << hold[i]<< endl;

   /* Open an existing file. */
   file_id = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

   /* Create the data space for the first dataset. */
   dims[0] = 4;
   dims[1] = 1;
   dataspace_id = H5Screate_simple(1, dims, NULL);

   /* split date */
   tokenizer<char_separator<char> > tokens(hold[0], sep);
   i = 0;
   BOOST_FOREACH (const string& t, tokens) {
       
      cout << t << "." << endl;
      date[i] = t;
      i++;
   }

   /* Group Based on Date */
  // string to const char* 
   year_group_id = H5Gcreate(file_id, date[0].c_str(),H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
   name = date[0] + "/" + date[1];
   month_group_id = H5Gcreate(file_id,name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
   day_group_id = H5Gcreate(file_id,hold[0].c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

   dims[0] = 1;
   dims[1] = 4;
   datatype = H5Tcopy(H5T_C_S1);
   H5Tset_size (datatype, H5T_VARIABLE);
   dataspace_id = H5Screate_simple(sizeof(dims)/sizeof(*dims), dims, NULL);

   /* Create a dataset in group "MyGroup". */
   dataset_id = H5Dcreate(day_group_id,"dset",H5T_C_S1, dataspace_id,
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

   datatype = H5Tcopy (H5T_C_S1);
   const char * s = hold[2].c_str ();
   status = H5Dwrite (dataset_id,datatype,H5S_ALL,H5S_ALL,H5P_DEFAULT,&s);

   status = H5Dclose(dataset_id);
   status = H5Gclose(year_group_id);

}

