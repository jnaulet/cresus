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

static int rsi_feed(struct indicator *i, struct track_n3 *e)
{  
  double h, b, sub;
  struct rsi *ctx = (void*)i;
  struct candle *c = (void*)e;
  
  if(!ctx->last)
    goto out;
  
  /* RSI formula :
   * 100.0 - (100.0 / (1 + h / b))
   * or
   * (h / (h + b)) * 100.0
   * where h is the ema of ups of last n days
   * and b is the fabs ema of downs of last n days
   */
  sub = c->price->close - ctx->last->price->close;
  if(sub > 0) h = average_update(&ctx->h, sub);
  if(sub < 0) b = average_update(&ctx->b, fabs(sub));
  /* Compute RSI the easy way */
  ctx->value = (h / (h + b)) * 100.0;
  
  /* TODO : add event management */
 out:
  ctx->last = c;
  return 0;
}

static void rsi_reset(struct indicator *i)
{
  struct rsi *ctx = (void*)i;
  /* RAZ */
  ctx->value = 0.0;
  ctx->last = NULL;
  /* Avg */
  average_reset(&ctx->h);
  average_reset(&ctx->b);
}

int rsi_init(struct rsi *ctx, unique_id_t id, int period)
{
  /* Super() */
  __indicator_init__(ctx, id, rsi_feed, rsi_reset);
  __indicator_set_string__(ctx, "rsi[%d]", period);
  
  ctx->value = 0.0;
  ctx->last = NULL;
  
  average_init(&ctx->h, AVERAGE_EXP, period);
  average_init(&ctx->b, AVERAGE_EXP, period);
  
  return 0;
}

void rsi_release(struct rsi *ctx)
{
  __indicator_release__(ctx);
  average_release(&ctx->h);
  average_release(&ctx->b);
}

double rsi_value(struct rsi *ctx)
{  
  return ctx->value;
}
