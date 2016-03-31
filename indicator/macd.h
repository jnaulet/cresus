#ifndef MACD_H
#define MACD_H

#include "math/average.h"
#include "framework/candle.h"
#include "framework/indicator.h"

struct macd_result {
  double value;
  double signal;
  double histogram;
};

struct macd {
  /* Parent */
  struct indicator parent;
  
  struct average fast;
  struct average slow;
  struct average signal;

  /* Private */
  struct macd_result result;
};

int macd_init(struct macd *m, int fast_p, int slow_p, int signal_p,
	      struct candle *cdl);

void macd_free(struct macd *m);

int macd_feed(struct indicator *i, const struct candle *c);

/* indicator-specific */
const char *macd_str(struct macd *m, char *buf, size_t len);

#endif
