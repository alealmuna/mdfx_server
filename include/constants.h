#ifndef INCLUDE_CONSTANTS_H_
#define INCLUDE_CONSTANTS_H_

#include <string>
#include <boost/bimap.hpp>

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

typedef struct {
  double day;
  double index;
} DataIndex;

extern boost::bimap<std::string, int> nemo_map;

// List of symbols to pre-process
#define NEMO_REGEXP "(GBPUSD|USDJPY|EURUSD)"

#endif  // INCLUDE_CONSTANTS_H_
