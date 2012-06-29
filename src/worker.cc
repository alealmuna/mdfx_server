#include <zmq.hpp>
#include <iostream>

#include "include/worker.h"
#include "include/proto_handler.h"
#include "include/csv_handler.h"
#include "include/hdf5_handler.h"
#include "include/constants.h"
#include "src/protobuf/interfaces.pb.h"

using std::cout;
using std::endl;
using std::vector;

void Worker::preprocessor(void) {
  CsvHandler csvhandler;
  vector<string> files;
  vector<Quote> quotes;

  files = csvhandler.readdir("/tmp/mdfx_data");
  cout << "files: " << files[0] << endl;
  quotes = csvhandler.readcsv(files);
  cout << "number of quotes: " << quotes.size() << endl;
  writeToH5(quotes);
}

void* Worker::listener(void *arg) {
  // Prepare our context and socket
  zmq::context_t *context = (zmq::context_t *) arg;
  zmq::socket_t socket(*context, ZMQ_REP);
  socket.connect("inproc://workers");

  ProtoHandler phandler;

  while (1) {
    // The response will be saved into a vector
    vector<Quote> quotes;
    float last_quote;

    // Allocation for zmq and protobuff 
    zmq::message_t request;
    mdfx_server::FXRequest pb_request;

    // receive the request, and parse the protocol buffer from it
    try {
      socket.recv(&request);
    } catch(zmq::error_t&) {
      cout << zmq_strerror(errno) << endl;
      break;
    }

    // process the request
    phandler.ProcessRequest(request, pb_request, quotes);
    last_quote = quotes.size() - 1;

    std::cout << "server: Received " << pb_request.begin_timestamp() <<
      ": " << pb_request.end_timestamp() << std::endl;


    // create a response
    mdfx_server::BBOFXQuote pb_response;
    std::string pb_serialized;

    // create the reply as a multipart message, transforming each vector's
    // element into a protobuf object
    for (size_t i = 0; i < last_quote; ++i) {
      pb_response.set_timestamp(quotes[i].tstamp);
      pb_response.set_symbol_nemo("dummy nemo");  // map pending
      pb_response.set_bid_price(quotes[i].bidp);
      pb_response.set_bid_size(quotes[i].bids);
      pb_response.set_ask_price(quotes[i].askp);
      pb_response.set_ask_size(quotes[i].asks);
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
    pb_response.set_timestamp(quotes[last_quote].tstamp);
    pb_response.set_symbol_nemo("dummy nemo");  // map pending
    pb_response.set_bid_price(quotes[last_quote].bidp);
    pb_response.set_bid_size(quotes[last_quote].bids);
    pb_response.set_ask_price(quotes[last_quote].askp);
    pb_response.set_ask_size(quotes[last_quote].asks);
    pb_response.SerializeToString(&pb_serialized);
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
