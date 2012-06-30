#ifndef INCLUDE_WORKER_H_
#define INCLUDE_WORKER_H_

void PreProcessData(void);

class Worker {
 public:
  static void *listener(void *arg);
};

#endif  // INCLUDE_WORKER_H_
