#include <iostream>
#include "proto_handler.h"

using std::cout;
using std::endl;

int main() {
  ProtoHandler phandler;
  cout << "Amucan Market Data FX" << endl;
  phandler.readMessageFromFile();
  return 0;
}
