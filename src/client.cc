#include <zmq.hpp>
#include <iostream>

#include "src/protobuf/interfaces.pb.h"

using std::cout;
using std::endl;

int main() {
  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);

  socket.connect("tcp://localhost:5555");

  for (int request_nbr = 0; request_nbr != 5; request_nbr++) {
    // set up the request protobuf
    mdfx_server::FXRequest pb_request;
    pb_request.set_begin_timestamp(1);
    pb_request.set_end_timestamp(5);

    // serialize the request to a string
    std::string pb_serialized;
    pb_request.SerializeToString(&pb_serialized);

    // create and send the zmq message
    zmq::message_t request(pb_serialized.size());
    memcpy(reinterpret_cast<void *>(request.data()), pb_serialized.c_str(),
      pb_serialized.size());

    cout << "client: Sending request " << request_nbr << "..." <<
      endl;
    socket.send(request);

    while (1) {
      //  create and receive the reply
      zmq::message_t reply;
      socket.recv(&reply);

      // get the response object and parse it
      mdfx_server::FXRequest pb_response;
      pb_response.ParseFromArray(reply.data(), reply.size());

      //assert(pb_response.begin_timestamp() == pb_request.begin_timestamp());
      //assert(pb_response.end_timestamp() == pb_request.end_timestamp());
      cout << "client: Received " << pb_response.begin_timestamp() << ": "
        << pb_response.end_timestamp() << endl;

      int64_t more;
      size_t more_size = sizeof (more);
      zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_size);
      if (!more)
        break;      //  Last message part
    }
  }

  cout << "Exiting..." << endl;

  return 0;
}
