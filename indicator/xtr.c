#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>

#include "xtr.h"

static double xtr_open(struct xtr *x)
{
  int index = (x->index + (x->period - 1)) % x->period;
  return x->pool[index].open;
}

static double xtr_close(struct xtr *x)
{
  int index = (x->index + (x->period - 1)) % x->period;
  return x->pool[index].close;
}

static double xtr_low(struct xtr *x)
{
  double low = DBL_MAX;

  for(int i = x->period; i--;)
    low = (x->pool[i].low < low ? x->pool[i].low : low);

  return low;
}

static double xtr_high(struct xtr *x)
{
  double high = 0;

  for(int i = x->period; i--;)
    high = (x->pool[i].high > high ? x->pool[i].high : high);

  return high;
}

int xtr_init(struct xtr *x, int period, struct candle *cdl)
{
  x->count = 1;
  x->index = 0;
  x->period = period;

  x->pool = malloc(sizeof(*x->pool) * period);
  memcpy(&x->pool[x->index++], cdl, sizeof *cdl);

  return !!x->pool;
}

void xtr_free(struct xtr *x)
{
  if(x->pool) free(x->pool);
}

struct candle *xtr_feed(struct xtr *x, struct candle *cdl)
{
  memcpy(&x->pool[x->index], cdl, sizeof *cdl);  
  x->index = (x->index + 1) % x->period;
  
  x->res.day = cdl->day;
  x->res.month = cdl->month;
  x->res.year = cdl->year;
  
  if(++x->count >= x->period){
    /* Fill result candle */
    x->res.open = xtr_open(x);
    x->res.close = xtr_close(x);
    x->res.high = xtr_high(x);
    x->res.low = xtr_low(x);

    return &x->res;
  }
  
  return NULL;
}

struct candle *xtr_compute(struct xtr *x)
{
  if(x->count >= x->period)
    return &x->res;
  
  return NULL;
}
