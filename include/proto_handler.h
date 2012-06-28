#ifndef INCLUDE_PROTO_HANDLER_H_
#define INCLUDE_PROTO_HANDLER_H_

#include <zmq.hpp>

#include "src/protobuf/interfaces.pb.h"
#include "constants.h"

using std::vector;

class ProtoHandler {
 public:
  // Reads a zmq message, parsing into to a protobuf and communicates with the
  // HDF5 utilities to process the request
  void ProcessRequest(
      zmq::message_t &,
      mdfx_server::FXRequest &,
      vector <Quote> &);
};

#endif  // INCLUDE_PROTO_HANDLER_H_
