#include <pthread.h>
#include <unistd.h>
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

void *worker_routine(void *arg) {
  // Prepare our context and socket
  zmq::context_t *context = (zmq::context_t *) arg;
  zmq::socket_t socket(*context, ZMQ_REP);
  socket.connect("inproc://workers");

  ProtoHandler phandler;

  phandler.readRequestFromFile("test_request");

  while (!s_interrupted) {
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
    // To test this, mdfx_server::FXRequest is used as response but it should
    // be mdfx_server::BBOFXQuote
    mdfx_server::FXRequest pb_response;
    std::string pb_serialized;

    // create the reply as a multipart message
    for (size_t i = 0; i < 10; ++i) {
      pb_response.set_begin_timestamp(pb_request.begin_timestamp());
      pb_response.set_end_timestamp(i);
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

int main() {
  cout << "Amucan Market Data FX" << endl;
  s_catch_signals();

  // Prepare our context and sockets
  zmq::context_t context(1);
  zmq::socket_t clients(context, ZMQ_ROUTER);
  clients.bind("tcp://*:5555");
  zmq::socket_t workers(context, ZMQ_DEALER);
  workers.bind("inproc://workers");

  // Launch pool of worker threads
  for (int thread_nbr = 0; thread_nbr != 2; thread_nbr++) {
    pthread_t worker;
    pthread_create(&worker, NULL, worker_routine,
      (reinterpret_cast<void *>(&context)));
    pthread_detach(worker);
  }
  // Connect work threads to client threads via a queue
  try {
    zmq::device(ZMQ_QUEUE, clients, workers);
  } catch(zmq::error_t&) {
    int errsv = errno;
    cout << zmq_strerror(errsv) << endl;
  }

  google::protobuf::ShutdownProtobufLibrary();
  cout << "Exiting..." << endl;
  cout << "Workers shutting down..." << endl;

  return 0;
}
