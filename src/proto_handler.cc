#include "include/proto_handler.h"

void ProtoHandler::ProcessRequest(
    zmq::message_t &request, 
    mdfx_server::FXRequest &pb_request,
    vector <Quote> &quotes) {

    pb_request.ParseFromArray(request.data(), request.size());

    // Dummy quote for testing purposes
    Quote quote;

    quote.tstamp = 1;
    quote.nemo = 1;
    quote.bidp = 100;
    quote.bids = 100;
    quote.askp = 100;
    quote.asks = 1000;

    quotes.push_back(quote);
    quote.tstamp = 2;
    quotes.push_back(quote);
}
