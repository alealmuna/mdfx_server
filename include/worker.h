#ifndef INCLUDE_WORKER_H_
#define INCLUDE_WORKER_H_

class Worker {
  public:
    static void *worker_routine(void *arg);
};

#endif  // INCLUDE_WORKER_H_
