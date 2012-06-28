#ifndef INCLUDE_SERVER_H_
#define INCLUDE_SERVER_H_

class Server {
  public:
    static void *worker_routine(void *arg);
};

#endif  // INCLUDE_SERVER_H_
