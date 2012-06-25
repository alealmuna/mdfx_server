#include <signal.h>
#include <iostream>

#include "include/proto_handler.h"

using std::cout;
using std::endl;

static int s_interrupted = 0;
static void s_signal_handler(int /*signal_value*/) {
    s_interrupted = 1;
}

static void s_catch_signals(void) {
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

int main() {
  ProtoHandler phandler;

  s_catch_signals();

  while (1) {
    cout << "Amucan Market Data FX" << endl;
    phandler.readRequestFromFile("test_request");

    if (s_interrupted) {
      cout << "W: interrupt received, killing server..." << endl;
      break;
    }
  }
  cout << "Cleaning..." << endl;
  return 0;
}
