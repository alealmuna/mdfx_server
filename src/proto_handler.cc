#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/bimap.hpp>

#include "include/proto_handler.h"
#include "include/hdf5_handler.h"

using std::vector;
using std::string;
using std::ios;
using std::cout;
using std::cerr;
using std::endl;
using std::fstream;
using boost::bimap;

void ProtoHandler::ProcessRequest(
    zmq::message_t &request, 
    mdfx_server::FXRequest &pb_request,
    vector <Quote> &quotes) {

    // converting from zmq message to protobuf
    pb_request.ParseFromArray(request.data(), request.size());

    // converting from protobuf to Fxrequest struct
    Fxrequest fxrequest;
    fxrequest.begin_ts = pb_request.begin_timestamp();
    fxrequest.end_ts = pb_request.end_timestamp();
    fxrequest.max_rel_spread = pb_request.max_rel_spread();

    int nemo_size = pb_request.nemo_list_size();
    
    fxrequest.nemo = new int[nemo_size];

    bimap<string, int> nemo_map;
    nemo_map.insert(bimap<string, int>::value_type("EURUSD", 0));
    nemo_map.insert(bimap<string, int>::value_type("GBPUSD", 1));
    nemo_map.insert(bimap<string, int>::value_type("USDJPY", 2));

    for (int i = 0; i < pb_request.nemo_list_size(); i++) {
      bimap<string, int>::left_const_iterator nemo_iterator;
      nemo_iterator = nemo_map.left.find(pb_request.nemo_list(i));
      fxrequest.nemo[i] = nemo_iterator->second;
    }

    ProcessResponse(fxrequest, quotes);
}

bool ProtoHandler::ReadRequestFromFile(
    string filename, 
    mdfx_server::FXRequest &pb_request) {
  
  fstream input(filename.c_str(), ios::in | ios::binary);

  if (!input) {
    cout << filename << ": File not found." << endl;
    return false;
  }
  if (!pb_request.ParseFromIstream(&input)){
    cerr << "Failed to parse :" << filename << endl;
    return false;
  }
  
  cout << "Reading protobuf message from " << filename << endl;
  cout << "  begin_timestamp:" << pb_request.begin_timestamp() << endl;
  cout << "  end_timestamp:" << pb_request.end_timestamp() << endl;
  cout << "  max_rel_spread:" << pb_request.max_rel_spread() << endl;
  cout << "  nemos:" << endl;
  for ( int i = 0; i < pb_request.nemo_list_size(); i++){
    cout << "    " <<  pb_request.nemo_list(i) << endl;
  }
  return true;
}
