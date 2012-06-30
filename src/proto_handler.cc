#include <vector>

#include "include/proto_handler.h"
#include "include/hdf5_handler.h"

using std::vector;

void ProtoHandler::ProcessRequest(
    zmq::message_t &request, 
    mdfx_server::FXRequest &pb_request,
    vector <Quote> &quotes) {

    // converting from zmq message to protobuf
    pb_request.ParseFromArray(request.data(), request.size());

    readFromH5(quotes);
    // converting from protobuf to Fxrequest struct
    //Fxrequest fxrequest;
    //fxrequest.begin_ts = pb_request.begin_timestamp();
    //fxrequest.end_ts = pb_request.end_timestamp();
    //fxrequest.max_rel_spread = pb_request.max_rel_spread();
    //// TODO nemo_list pending

    //processResponse(fxrequest, quotes);
}
