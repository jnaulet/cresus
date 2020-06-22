/*
 * Cresus EVO - bollinger.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include "bollinger.h"

static int bollinger_feed(struct indicator *i, struct track_n3 *e)
{  
  struct bollinger *ctx = (void*)i;
  struct candle *c = (void*)e;
  double value = candle_get_value(c, ctx->cvalue);
  ctx->value.mma = average_update(&ctx->avg, value);

  if(average_is_available(&ctx->avg)){
    double stddev = average_stddev(&ctx->avg);
    ctx->value.hi = ctx->value.mma + ctx->stddev_factor * stddev;
    ctx->value.lo = ctx->value.mma - ctx->stddev_factor * stddev;
  }
  
  return (ctx->value.mma != 0.0 ? 1 : 0);
}

static void bollinger_reset(struct indicator *i)
{
  struct bollinger *ctx = (void*)i;
  average_reset(&ctx->avg);
}

int bollinger_init(struct bollinger *ctx, unique_id_t id, int period,
                   double stddev_factor, candle_value_t cvalue)
{  
  /* init */
  __indicator_init__(ctx, id, bollinger_feed, bollinger_reset);
  __indicator_set_string__(ctx, "boll[%d, %.1f]", period, stddev_factor);
  
  ctx->stddev_factor = stddev_factor;
  ctx->cvalue = cvalue;
  average_init(&ctx->avg, AVERAGE_MATH, period);
  
  return 0;
}

void bollinger_release(struct bollinger *ctx)
{
  __indicator_release__(ctx);
  average_release(&ctx->avg);
}


struct bollinger_value *ctxollinger_get_value(struct bollinger *ctx)
{
  return (ctx->value.mma != 0.0 ? &ctx->value : NULL);
}

const char *ctxollinger_str(struct bollinger *ctx)
{
  sprintf(ctx->str, "%.2lf %.2lf:%.2lf", ctx->value.mma,
	  ctx->value.hi, ctx->value.lo);

  return ctx->str;
}
