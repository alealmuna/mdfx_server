#include "proto_handler.h"

using namespace std;

int main()
{
  ProtoHandler phandler;
  string fname ("test_message");
  cout << "Amucan Market Data FX" << endl;
  phandler.readMessageFromFile();
  return 0;
}
