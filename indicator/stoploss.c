/*
 * Cresus EVO - stoploss.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/17/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "stoploss.h"
#include "engine/candle.h"

static int stoploss_feed(struct indicator *i, struct track_n3 *e)
{  
  struct stoploss *ctx = (void*)i;
  struct candle *candle = (void*)e;
  
  if(ctx->type == STOPLOSS_UP){
    if(candle->price->close >= ctx->value){
      if(ctx->trigger)
        goto out;
    }
    
  }else{
    if(candle->price->close <= ctx->value){
      if(ctx->trigger)
        goto out;
    }
  }
  
  return 0;
  
out:
  //indicator_set_event(i, candle, STOPLOSS_EVENT_HIT);
  stoploss_clear(ctx);
  return 1;
}

static void stoploss_reset(struct indicator *i)
{
  struct stoploss *ctx = (void*)i;
  ctx->value = 0.0;
  ctx->trigger = 0;
}

int stoploss_init(struct stoploss *ctx, unique_id_t id, double percent)
{  
  /* Init parent */
  __indicator_init__(ctx, id, stoploss_feed, stoploss_reset);
  __indicator_set_string__(ctx, "stoploss[%.2f]", percent);
  
  ctx->value = 0.0;
  ctx->trigger = 0;
  ctx->percent = percent;
  
  return 0;
}

void stoploss_release(struct stoploss *ctx) {
  
  __indicator_release__(ctx);
  ctx->value = 0.0;
  ctx->trigger = 0;
}

void stoploss_set(struct stoploss *ctx, stoploss_t type, double value)
{  
  if(type == STOPLOSS_UP)
    ctx->value = value * (1.0 + (ctx->percent / 100.0));
  else
    ctx->value = value * (1.0 - (ctx->percent / 100.0));
  
  /* Trigger signal */
  ctx->trigger = 1;
  ctx->type = type;
}

void stoploss_clear(struct stoploss *ctx)
{  
  ctx->value = 0.0;
  ctx->trigger = 0;
}
