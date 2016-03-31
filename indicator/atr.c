#include <math.h>
#include <stdio.h>
#include <string.h>

#include "atr.h"

int atr_init(struct atr *a, int period, const struct candle *c)
{
  /* Super() */
  indicator_init(&a->parent, CANDLE_CLOSE, atr_feed);
  
  a->period = period;
  a->value = c->high - c->low;
  memcpy(&a->last, c, sizeof *c);
  
  return 0;
}

void atr_free(struct atr *a)
{
  indicator_free(&a->parent);
}

int atr_feed(struct atr *a, const struct candle *c)
{
  /* Compute "True Range" */
  double tr = c->high - c->low;
  double h = fabs(c->high - a->last.close);
  double l = fabs(c->low - a->last.close);
  
  tr = (h > tr ? h : tr);
  tr = (l > tr ? l : tr);

  /* Average it */
  a->value = (a->value * (a->period - 1) + tr) / a->period;
  
  /* Remember last candle */
  memcpy(&a->last, c, sizeof *c);
  return 0;
}

double atr_value(struct atr *a)
{
  return a->value;
}
