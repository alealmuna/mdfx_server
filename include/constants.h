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
