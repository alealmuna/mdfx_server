#include "include/proto_handler.h"
#include "include/hdf5_handler.h"

void ProtoHandler::ProcessRequest(
    zmq::message_t &request, 
    mdfx_server::FXRequest &pb_request,
    vector <Quote> &quotes) {

    pb_request.ParseFromArray(request.data(), request.size());

    readFromH5(quotes);
}
