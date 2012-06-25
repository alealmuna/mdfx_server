#include "hdf5.h"
#define FILE "file.h5"
#define file_id 5
/* Filename */
#define FILENAME "accum.h5"

int main() {
  hid_t       file_id;   /* file identifier */
  herr_t      status;

  /* Create a new file using default properties. */
  file_id = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  /* Terminate access to the file. */
  status = H5Fclose(file_id); 
}

