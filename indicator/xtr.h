#ifndef XTR_H
#define XTR_H

#include "framework/candle.h"
#include "framework/indicator.h"

struct xtr {

  /* This is no indicator, just a data aggregator
   * Dunno what to do with this thing */
  
  int count;
  int index;
  int period;

  struct candle *pool;
  struct candle res;
};

int xtr_init(struct xtr *x, int period, struct candle *cdl);
void xtr_free(struct xtr *x);

struct candle *xtr_feed(struct xtr *x, struct candle *cdl);
struct candle *xtr_compute(struct xtr *x);
void xtr_reset(struct xtr *x);

#endif
