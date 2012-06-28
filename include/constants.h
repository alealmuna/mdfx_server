#ifndef INCLUDE_CONSTANTS_H_
#define INCLUDE_CONSTANTS_H_

enum ENUM_NEMO {
  EUR_USD,
  GBP_USD,
  USD_JPY
};

typedef struct {
  float tstamp;
  int   nemo;
  float bidp;
  float bids;
  float askp;
  float asks;
} Quote;

#endif  // INCLUDE_CONSTANTS_H_
