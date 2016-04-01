/*
 * Cresus EVO - macd.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef MACD_H
#define MACD_H

#include "math/average.h"
#include "framework/indicator.h"

struct macd_result {
  double value;
  double signal;
  double histogram;
};

/* Indicator events */
#define MACD_EVENT_SIGNAL0 0 /* TODO */
#define MACD_EVENT_SIGNAL1 0 /* TODO */
#define MACD_EVENT_SIGNAL2 0 /* TODO */
#define MACD_EVENT_SIGNAL3 0 /* TODO */

/* Default params */
#define MACD_DEFAULT_FAST_P   12
#define MACD_DEFAULT_SLOW_P   26
#define MACD_DEFAULT_SIGNAL_P 9

struct macd {
  /* Parent */
  __inherits_from_indicator__;
  
  struct average fast;
  struct average slow;
  struct average signal;

  /* Private */
  struct macd_result result;
};

int macd_init(struct macd *m, int fast_p, int slow_p, int signal_p);
void macd_free(struct macd *m);

/* indicator-specific */
const char *macd_str(struct macd *m, char *buf, size_t len);

#endif
