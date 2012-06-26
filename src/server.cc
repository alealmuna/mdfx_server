#include <signal.h>
#include <zmq.hpp>
#include <iostream>

#include "include/proto_handler.h"
#include "src/protobuf/interfaces.pb.h"

using std::cout;
using std::endl;

static int s_interrupted = 0;
static void s_signal_handler(int /*signal_value*/) {
    s_interrupted = 1;
}

static void s_catch_signals(void) {
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

int main() {
  ProtoHandler phandler;

  s_catch_signals();

  cout << "Amucan Market Data FX" << endl;
  phandler.readRequestFromFile("test_request");

  //  Prepare our context and socket
  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REP);
  socket.bind("tcp://*:5555");

  while (!s_interrupted) {
    zmq::message_t request;
    mdfx_server::FXRequest pb_request;

    // receive the request, and parse the protocol buffer from it
    try {
      socket.recv(&request);
    } catch(zmq::error_t&) {
      cout << "catched!" << endl;
      break;
    }

    pb_request.ParseFromArray(request.data(), request.size());

    std::cout << "server: Received " << pb_request.begin_timestamp() <<
      ": " << pb_request.end_timestamp() << std::endl;

    // process the request
    // ...

    // create a response
    // To test this, mdfx_server::FXRequest is used as response but it should
    // be mdfx_server::BBOFXQuote
    mdfx_server::FXRequest pb_response;
    pb_response.set_begin_timestamp(pb_request.begin_timestamp());
    pb_response.set_end_timestamp(pb_request.end_timestamp());
    std::string pb_serialized;
    pb_response.SerializeToString(&pb_serialized);

    //  create the reply
    zmq::message_t reply(pb_serialized.size());
    memcpy(reinterpret_cast<void *>(reply.data()), pb_serialized.c_str(),
        pb_serialized.size());
    try {
      socket.send(reply);
    } catch(zmq::error_t&) {
      cout << "catched!" << endl;
      break;
    }
  }

  cout << "Exiting..." << endl;

  return 0;
}
