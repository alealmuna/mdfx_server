#include <iostream>
#include <fstream>
#include "include/proto_handler.h"
#include "src/protobuf/interfaces.pb.h"

using std::cout;
using std::cerr;
using std::ios;
using std::endl;
using std::fstream;

bool ProtoHandler::readRequestFromFile(string filename) {
  fstream input(filename.c_str(), ios::in | ios::binary);
  mdfx_server::FXRequest request;
  request.ParseFromIstream(&input);
  if (!input) {
    cerr << "Failed to parse "  << filename << endl;
//    return false;
  }
  
  cout << "Reading protobuf message from file: " << filename << endl;
  cout << "  begin_timestamp:" << request.begin_timestamp() << endl;
  cout << "  end_timestamp:" << request.end_timestamp() << endl;
  cout << "  max_rel_spread:" << request.max_rel_spread() << endl;
//  for ( int i = 0; i < request.nemo_list.size(); i++){
//    cout << request.nemo_list(i) << endl;
//  }
  return true;
}
