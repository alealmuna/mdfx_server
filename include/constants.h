#ifndef INCLUDE_CONSTANTS_H_
#define INCLUDE_CONSTANTS_H_

enum ENUM_NEMO {
  EUR_USD,
  GBP_USD,
  USD_JPY
};

typedef struct {
  double tstamp;
  int   nemo;
  float bidp;
  float bids;
  float askp;
  float asks;
} Quote;


typedef struct {
  double begin_ts;
  double end_ts;
  double max_rel_spread;
  int *nemo;
} Fxrequest;


#endif  // INCLUDE_CONSTANTS_H_
