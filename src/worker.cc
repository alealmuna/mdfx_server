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
using std::string;

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

    // Allocation for zmq and protobuff 
    zmq::message_t request;
    mdfx_server::FXRequest pb_request;

    // receive the request
    try {
      socket.recv(&request);
    } catch(zmq::error_t&) {
      cout << zmq_strerror(errno) << endl;
      break;
    }

    // process the request
    phandler.ProcessRequest(request, pb_request, quotes);

    std::cout << "server: Received " << pb_request.begin_timestamp() <<
      ": " << pb_request.end_timestamp() << std::endl;

    // create a response
    mdfx_server::BBOFXQuote pb_response;
    std::string pb_serialized;

    // create the reply as a multipart message, transforming each vector's
    // element into a protobuf object and erasing it after
    std::vector<Quote>::iterator it = quotes.begin();
    while (it != quotes.end()) {
      pb_response.set_timestamp(it->tstamp);
      pb_response.set_symbol_nemo("dummy nemo");  // map pending
      pb_response.set_bid_price(it->bidp);
      pb_response.set_bid_size(it->bids);
      pb_response.set_ask_price(it->askp);
      pb_response.set_ask_size(it->asks);

      // on-the-fly resource release
      it = quotes.erase(it);

      pb_response.SerializeToString(&pb_serialized);
      zmq::message_t reply(pb_serialized.size());
      memcpy(reinterpret_cast<void *>(reply.data()), pb_serialized.c_str(),
          pb_serialized.size());

      // zmq message reply
      try {
        if (it != quotes.end()) {
          socket.send(reply, ZMQ_SNDMORE);
        } else {
          // final message part
          socket.send(reply);
        }
      } catch(zmq::error_t&) {
        cout << zmq_strerror(errno) << endl;
        break;
      }
    }
  }
  return (NULL);
}
