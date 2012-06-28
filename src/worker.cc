#include <zmq.hpp>
#include <iostream>

#include "include/worker.h"
#include "include/proto_handler.h"
#include "src/protobuf/interfaces.pb.h"

using std::cout;
using std::endl;

void *Worker::worker_routine(void *arg) {
  // Prepare our context and socket
  zmq::context_t *context = (zmq::context_t *) arg;
  zmq::socket_t socket(*context, ZMQ_REP);
  socket.connect("inproc://workers");

  ProtoHandler phandler;

  phandler.readRequestFromFile("test_request");

  while (1) {
    zmq::message_t request;
    mdfx_server::FXRequest pb_request;

    // receive the request, and parse the protocol buffer from it
    try {
      socket.recv(&request);
    } catch(zmq::error_t&) {
      cout << zmq_strerror(errno) << endl;
      break;
    }

    pb_request.ParseFromArray(request.data(), request.size());

    std::cout << "server: Received " << pb_request.begin_timestamp() <<
      ": " << pb_request.end_timestamp() << std::endl;

    // process the request
    // ...

    // create a response
    mdfx_server::BBOFXQuote pb_response;
    std::string pb_serialized;

    // create the reply as a multipart message
    for (size_t i = 0; i < 10; ++i) {
      // dummy set
      pb_response.set_timestamp(i);
      pb_response.SerializeToString(&pb_serialized);
      zmq::message_t reply(pb_serialized.size());
      memcpy(reinterpret_cast<void *>(reply.data()), pb_serialized.c_str(),
          pb_serialized.size());
      try {
        socket.send(reply, ZMQ_SNDMORE);
      } catch(zmq::error_t&) {
        cout << zmq_strerror(errno) << endl;
        break;
      }
    }
    // send the final part
    zmq::message_t reply(pb_serialized.size());
    memcpy(reinterpret_cast<void *>(reply.data()), pb_serialized.c_str(),
        pb_serialized.size());
    try {
      socket.send(reply);
    } catch(zmq::error_t&) {
      cout << zmq_strerror(errno) << endl;
      break;
    }
  }
  return (NULL);
}
