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
  struct rsi *ctx = (void*)i;
  
  /* Get relative n3 */
  struct rsi_n3 *n3;
  double h, b, sub, value;
  struct track_n3 *last = track_n3_prev(e, ctx->period);
  
  /* RSI formula :
   * 100.0 - (100.0 / (1 + h / b))
   * or
   * (h / (h + b)) * 100.0
   * where h is the ema of ups of last n days
   * and b is the fabs ema of downs of last n days
   */
  sub = e->quotes->close - last->quotes->close;
  if(sub > 0) h = average_update(&ctx->h, sub);
  if(sub < 0) b = average_update(&ctx->b, fabs(sub));
  
  /* Compute RSI the easy way */
  value = (h / (h + b)) * 100.0;
  
  if(rsi_n3_alloc(n3, i, value)){
    track_n3_add_indicator_n3(e, &n3->indicator_n3);
    return 1;
  }
  
  return 0;
}

static void rsi_reset(struct indicator *i)
{
  struct rsi *ctx = (void*)i;
  /* Avg */
  average_reset(&ctx->h);
  average_reset(&ctx->b);
}

int rsi_init(struct rsi *ctx, unique_id_t uid, int period)
{
  /* Super() */
  indicator_init(&ctx->indicator, uid, rsi_feed, rsi_reset);
  indicator_set_string(&ctx->indicator, "rsi[%d]", period);
  
  average_init(&ctx->h, AVERAGE_EXP, period);
  average_init(&ctx->b, AVERAGE_EXP, period);
  
  ctx->period = period;
  return 0;
}

void rsi_release(struct rsi *ctx)
{
  indicator_release(&ctx->indicator);
  average_release(&ctx->h);
  average_release(&ctx->b);
}
