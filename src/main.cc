#include <pthread.h>
#include <signal.h>
#include <zmq.hpp>
#include <google/protobuf/stubs/common.h>
#include <iostream>

#include "include/worker.h"

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
  cout << "Amucan Market Data FX" << endl;
  s_catch_signals();

  // Worker responsible of preprocessing
  Worker initializer;
  initializer.preprocessor();

  // Prepare our context and sockets
  zmq::context_t context(1);
  zmq::socket_t clients(context, ZMQ_ROUTER);
  clients.bind("tcp://*:5555");
  zmq::socket_t workers(context, ZMQ_DEALER);
  workers.bind("inproc://workers");

  // Launch pool of worker threads
  for (int thread_nbr = 0; thread_nbr != 2; thread_nbr++) {
    pthread_t worker;
    pthread_create(&worker, NULL, Worker::listener,
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
