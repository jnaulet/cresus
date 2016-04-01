/*
 * Cresus EVO - rsi.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdlib.h>

#include "rsi.h"
#include "mobile.h"

static int rsi_feed(struct indicator *i, struct candle *c) {
  
  double h, b, sub;
  struct rsi *r = __indicator_self__(i);

  if(!r->last)
    goto out;
  
  /* RSI formula :
   * 100.0 - (100.0 / (1 + h / b))
   * or
   * (h / (h + b)) * 100.0
   * where h is the ema of ups of last n days
   * and b is the fabs ema of downs of last n days
   */
  sub = c->close - r->last->close;
  if(sub > 0) h = average_update(&r->h, sub);
  if(sub < 0) b = average_update(&r->b, fabs(sub));
  /* Compute RSI the easy way */
  r->value = (h / (h + b)) * 100.0;
  
  /* TODO : add event management */
 out:
  r->last = c;
  return 0;
}

int rsi_init(struct rsi *r, int period)
{
  /* Super() */
  __indicator_super__(r, rsi_feed);
  __indicator_set_string__(r, "rsi[%d]", period);
  
  r->value = 0.0;
  r->last = NULL;
  
  average_init(&r->h, AVERAGE_EXP, period);
  average_init(&r->b, AVERAGE_EXP, period);
  
  return 0;
}

void rsi_free(struct rsi *r)
{
  __indicator_free__(r);
  average_free(&r->h);
  average_free(&r->b);
}

double rsi_value(struct rsi *r) {

  return r->value;
}
