#include <iostream>
#include "include/proto_handler.h"
#include "src/protobuf/interfaces.pb.h"

using std::cout;
using std::endl;
using std::string;

bool ProtoHandler::readRequestFromFile(string request) {
  cout << "Reading protobuf message from file: " << request  << endl;
  return false;
}
