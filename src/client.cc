#include <cstdio>
#include <iostream>
#include <string>
#include <zmq.hpp>

using std::cout;
using std::endl;

#include "src/protobuf/interfaces.pb.h"
#include "include/proto_handler.h"

int main(int argc, char* argv[]) {
  // Wrong arguments case
  if ((argc != 1) || (argc == 3 && !strcmp(argv[1], "-proto"))){
    cout << "Wrong parameters" << endl;
    cout << "usage: " << endl;
    cout << "       " << "mdfx_client" << endl;
    cout << "       " << "mdfx_client -proto <filename>" << endl;
    return 0;
  }
  printf("Starting client...\n");

  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);

  socket.connect("tcp://localhost:5555");

  mdfx_server::FXRequest pb_request;
  ProtoHandler phandler;
  // Load request from file 
  if (argc == 3 && !strcmp(argv[1], "-proto")) {
    std::string filename(argv[2]);
    cout << "Loading request from file :" << filename << endl;
    if(!phandler.ReadRequestFromFile(filename, pb_request)) return 0;
  } else {
    cout << "Using test request"  << endl;
    // set up the request protobuf
    pb_request.set_begin_timestamp(1293874200);
    pb_request.set_end_timestamp(1320080700);
    pb_request.set_max_rel_spread(0.01);
    pb_request.add_nemo_list("EURUSD");
    pb_request.add_nemo_list("GBPUSD");
    pb_request.add_nemo_list("USDJPY");
    
    cout << "  begin_timestamp: 1293874200" << endl;
    cout << "  end_timestamp: 1320080700" << endl;
    cout << "  max_rel_spread: 0.01" << endl;
    cout << "  nemos:" << endl;
    cout << "         EURUSD" << endl; 
    cout << "         GBPUSD" << endl; 
    cout << "         USDJPY" << endl; 
  }

  // serialize the request to a string
  std::string pb_serialized;
  pb_request.SerializeToString(&pb_serialized);

  // create and send the zmq message
  zmq::message_t request(pb_serialized.size());
  memcpy(reinterpret_cast<void *>(request.data()), pb_serialized.c_str(),
      pb_serialized.size());

  printf("Sending request...\n");
  socket.send(request);

  printf("Waiting for response...\n");
  while (1) {
    // create and receive the reply
    zmq::message_t reply;
    socket.recv(&reply);

    // get the response object and parse it
    mdfx_server::BBOFXQuote pb_response;
    pb_response.ParseFromArray(reply.data(), reply.size());

    // print each quote received
    printf("%15li, %10s, %10f, %10i, %10f, %10i",
        pb_response.timestamp(),
        pb_response.symbol_nemo().c_str(),
        pb_response.bid_price(),
        pb_response.bid_size(),
        pb_response.ask_price(),
        pb_response.ask_size());

    int64_t more;
    size_t more_size = sizeof(more);
    zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
    if (!more)
      break;  //  Last message part
  }

  printf("Exiting...\n");

  return 0;
}
