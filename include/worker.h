#ifndef INCLUDE_WORKER_H_
#define INCLUDE_WORKER_H_

class Worker {
 public:
  void preprocessor(void);
  static void *listener(void *arg);
};

#endif  // INCLUDE_WORKER_H_
