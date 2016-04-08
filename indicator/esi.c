/*
 * Cresus EVO - esi.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "esi.h"

int esi_init(struct esi *e, mobile_t type, int smooth_p)
{
  e->last = 0;
  return mobile_init(&e->smooth, type, smooth_p, e->last);
}

void esi_release(struct esi *e)
{
  mobile_release(&e->smooth);
}

double esi_feed(struct esi *e, double value)
{
  double ret = mobile_feed(&e->smooth, value - e->last);
  e->last = value;

  return ret;
}

double esi_compute(struct esi *e)
{
  return mobile_average(&e->smooth);
}
