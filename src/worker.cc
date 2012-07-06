#include <zmq.hpp>
#include <boost/bimap.hpp>
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
using boost::bimap;

void PreProcessData(void) {
  CsvHandler csvhandler;
  vector<string> files;
  vector<Quote> quotes;

  files = csvhandler.readdir("data");
  quotes = csvhandler.readcsv(files);
  cout << "[Loading Finished] Number of quotes: " << quotes.size() << endl;
  csvhandler.sortvec(quotes);
  cout << "Quotes sorted" << endl;
  writeToH5perDay(quotes);
  // createIndex(quotes, "data/MDFXIndexes.h5");

  // vector resources release
  if (!quotes.empty())
    quotes.clear();
}

void* Worker::listener(void *arg) {
  // Prepare our context and socket
  zmq::context_t *context = (zmq::context_t *) arg;
  zmq::socket_t socket(*context, ZMQ_REP);
  socket.connect("inproc://workers");

  ProtoHandler phandler;

  // map initialization
  bimap<string, int> nemo_map;
  nemo_map.insert(bimap<string, int>::value_type("EURUSD", 0));
  nemo_map.insert(bimap<string, int>::value_type("GBPUSD", 1));
  nemo_map.insert(bimap<string, int>::value_type("USDJPY", 2));

  while (!s_interrupted) {
    // The response will be saved into a vector
    vector<Quote> quotes;
    float last_quote;

    // Allocation for zmq and protobuff
    zmq::message_t request;
    mdfx_server::FXRequest pb_request;

    // create a response
    mdfx_server::BBOFXQuote pb_response;
    std::string pb_serialized;

    // receive the request
    try {
      socket.recv(&request);
    } catch(zmq::error_t&) {
      cout << zmq_strerror(errno) << endl;
      break;
    }

    // process the request
    phandler.ProcessRequest(request, pb_request, quotes);
    last_quote = quotes.size() - 1;  // we need the index in the vector

    cout << "server: Received " << pb_request.begin_timestamp() <<
      " : " << pb_request.end_timestamp() << endl;
    cout << "Number of quotes to send: " << quotes.size() <<endl;

    if (quotes.size() == 0) {
      // if nothing is found, a null message is sent
      pb_response.set_timestamp(-1);
      pb_response.SerializeToString(&pb_serialized);
      zmq::message_t reply(pb_serialized.size());
      memcpy(reinterpret_cast<void *>(reply.data()), pb_serialized.c_str(),
          pb_serialized.size());
      try {
        socket.send(reply);  // null message sent
      } catch(zmq::error_t&) {
        cout << zmq_strerror(errno) << endl;
        break;
      }
      pb_serialized.clear();
    } else {
      // create the reply as a multipart message, transforming each vector's
      // element into a protobuf object
      for (size_t i = 0; i < last_quote; ++i) {
        pb_response.set_timestamp(quotes[i].tstamp);
        // mapping nemo
        bimap<string, int>::right_const_iterator nemo_iterator;
        nemo_iterator = nemo_map.right.find(quotes[i].nemo);
        pb_response.set_symbol_nemo(nemo_iterator->second);
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
        pb_serialized.clear();
      }
      // send the final part
      // mapping nemo
      bimap<string, int>::right_const_iterator nemo_iterator;
      nemo_iterator = nemo_map.right.find(quotes[last_quote].nemo);
      pb_response.set_symbol_nemo(nemo_iterator->second);
      pb_response.set_timestamp(quotes[last_quote].tstamp);
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
      pb_serialized.clear();

      // vector resources release
      if (!quotes.empty())
        quotes.clear();
      // delete all global objects allocated by libprotobuf
      google::protobuf::ShutdownProtobufLibrary();
    }
  }
  return (NULL);
}
