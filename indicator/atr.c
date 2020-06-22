/*
 * Cresus EVO - atr.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>

#include "atr.h"

static int atr_feed(struct indicator *i, struct track_n3 *e)
{
  struct atr *a = (void*)i;
  struct candle *c = (void*)e;

  if(!a->ref){
    a->value = c->price->high - c->price->low;
    goto out;
  }
  
  /* Compute "True Range" */
  double tr = c->price->high - c->price->low;
  double h = fabs(c->price->high - a->ref->price->close);
  double l = fabs(c->price->low - a->ref->price->close);
  
  tr = (h > tr ? h : tr);
  tr = (l > tr ? l : tr);
  
  /* Average it */
  a->value = (a->value * (a->period - 1) + tr) / a->period;
  
  /* Remember last candle */
 out:
  a->ref = c;
  return 0;
}

static void atr_reset(struct indicator *i)
{
  struct atr *a = (void*)i;
  a->ref = NULL;
}

int atr_init(struct atr *a, unique_id_t id, int period)
{  
  /* Super() */
  __indicator_init__(a, id, atr_feed, atr_reset);
  __indicator_set_string__(a, "atr[%d]", period);
  
  a->ref = NULL;
  a->period = period;

  /* First seed */
  /* a->value = seed->price->high - seed->price->low; */
  
  return 0;
}

void atr_release(struct atr *a)
{
  __indicator_release__(a);
}

double atr_value(struct atr *a)
{
  return a->value;
}
