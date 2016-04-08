/*
 * Cresus EVO - esi.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ESI_H
#define ESI_H

/*
 * Exponential Strength Index
 * Find a better name, like Strength moving Average
 */

#include "framework/candle.h"
#include "mobile.h"

struct esi {
  double last;
  struct mobile smooth;
};

int esi_init(struct esi *e, mobile_t type, int smooth_p);
void esi_release(struct esi *e);

double esi_feed(struct esi *e, double value);
double esi_compute(struct esi *e);

#endif
