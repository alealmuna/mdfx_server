#include <iostream>
#include "proto_handler.h"

using namespace std;

int main()
{
  ProtoHandler phandler;
  cout << "Amucan Market Data FX" << endl;
  phandler.readMessageFromFile();
  return 0;
}
