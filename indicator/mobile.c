/*
 * Cresus EVO - mobile.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mobile.h"

static int mobile_feed(struct indicator *i, struct track_n3 *e)
{  
  struct mobile_n3 *n3;
  struct mobile *ctx = (void*)i;
  
  /* Trying to get average values */
  double last_avg = average_value(&ctx->avg);
  double avg = average_update(&ctx->avg, price_n3_value(e->price, ctx->value));
  
  if(average_is_available(&ctx->avg)){
    /* Create new n3 */
    if(mobile_n3_alloc(n3, i, avg, (avg - last_avg))){
      track_n3_add_indicator_n3(e, &n3->indicator_n3);
      return 1;
    }
  }
  
  return 0;
}

static void mobile_reset(struct indicator *i)
{
  struct mobile *ctx = (void*)i;
  average_reset(&ctx->avg);
}

int mobile_init(struct mobile *ctx, unique_id_t uid, mobile_t type,
		int period, price_n3_value_t value)
{  
  /* Super */
  indicator_init(&ctx->indicator, uid, mobile_feed, mobile_reset);
  indicator_set_string(&ctx->indicator, "%cma[%d]",
		       ((type == MOBILE_EMA) ? 'e' : 'm'),
		       period);
  
  ctx->type = type;
  ctx->value = value;
  
  switch(ctx->type) {
  case MOBILE_MMA : average_init(&ctx->avg, AVERAGE_MATH, period); break;
  case MOBILE_EMA : average_init(&ctx->avg, AVERAGE_EXP, period); break;
  }
  
  return 0;
}

void mobile_release(struct mobile *ctx)
{
  indicator_release(&ctx->indicator);
  average_release(&ctx->avg);
}

double mobile_average(struct mobile *ctx)
{
  return average_value(&ctx->avg);
}

double mobile_stddev(struct mobile *ctx)
{
  return average_stddev(&ctx->avg);
}
