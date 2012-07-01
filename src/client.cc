#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <zmq.hpp>

using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::ios;
using std::fstream;
using std::string;

#include "src/protobuf/interfaces.pb.h"
#include "include/proto_handler.h"

void PromptForRequest(mdfx_server::FXRequest* request) {
  cout << "Enter begin timestamp: ";
  double begin_ts;
  cin >> begin_ts;
  cin.ignore(256, '\n');
  request->set_begin_timestamp(begin_ts);

  cout << "Enter end timestamp: ";
  double end_ts;
  cin >> end_ts;
  request->set_end_timestamp(end_ts);
  cin.ignore(256, '\n');
  
  cout << "Enter max_rel_spread: ";
  float max_rel_spread;
  cin >> max_rel_spread;
  request->set_max_rel_spread(max_rel_spread);
  cin.ignore(256, '\n');
  
  while (true) {
    cout << "Enter a nemo (or leave blank to finish): ";
    string nemo;
    getline(cin, nemo);
    if (nemo.empty()) {
      break;
    }
    request->add_nemo_list(nemo);
  }
}

int main(int argc, char* argv[]) {
  // Wrong arguments case
  if (((argc != 1) && (argc != 2) && !(argc == 3 
      && !strcmp(argv[1], "--create-request")))
      || ((argc == 2) && !strcmp(argv[1],"--create-request"))){
    cout << "Wrong parameters" << endl;
    cout << "usage: " << endl;
    cout << "       " << "mdfx_client" << endl;
    cout << "       " << "mdfx_client <request file>" << endl;
    cout << "       " << "mdfx_client --create-request <request file>" << endl;
    return 0;
  }

  printf("Starting client...\n");

  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);

  mdfx_server::FXRequest pb_request;
  ProtoHandler phandler;
  // Load request from file 
  if(argc == 3 && !strcmp(argv[1], "--create-request")){
    PromptForRequest(&pb_request);
    string filename(argv[2]);
    fstream output(filename.c_str(), ios::out | ios::trunc | ios::binary);
    if (!pb_request.SerializeToOstream(&output)) {
      cerr << "Failed to write request." << endl;
      return -1;
    }
    else {
      cout << endl << endl;
      cout << "  Request created on file:   " << filename << endl << endl;
      cout << "  Send the request by running:" << endl;
      cout << endl;
      cout << "                     mdfx_client --request " << filename << endl;
      cout << endl;
      string user_in("");
      do {
        cout << "Would you like to send the request now? [Y/n]";
        getline(cin, user_in);
        if (!strcmp(user_in.c_str(),"n"))  return 0;
        if (!strcmp(user_in.c_str(),"Y") || !strcmp(user_in.c_str(),"y") 
          || !strcmp(user_in.c_str(),"")) break;
      } while (1);
    }
  }
  else if ((argc == 2) && strcmp(argv[1], "--create-request")){
    string filename(argv[1]);
    if(!phandler.ReadRequestFromFile(filename, pb_request))return 0;
  } 
  else  
    PromptForRequest(&pb_request);
  

  socket.connect("tcp://localhost:5555");

  // serialize the request to a string
  string pb_serialized;
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
    printf("%15li, %10s, %10f, %10i, %10f, %10i\n",
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
