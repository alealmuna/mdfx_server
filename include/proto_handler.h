#ifndef INCLUDE_PROTO_HANDLER_H_
#define INCLUDE_PROTO_HANDLER_H_

#include <iostream>

using std::string;

class ProtoHandler {
  public:
    // Read a protocol buffer message from a file
    bool readRequestFromFile(string);
};

#endif  // INCLUDE_PROTO_HANDLER_H_
