/*
 * Cresus EVO - roc.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 02.05.2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdlib.h>

#include "roc.h"

static int roc_feed(struct indicator *i, struct timeline_track_n3 *e)
{
  struct roc *ctx = (void*)i;
  
  if(!i->is_empty){
    
    double value;
    struct roc_n3 *n3;
    
    if(roc_compute(ctx, e, &value) != -1){
      if(roc_n3_alloc(n3, i, value)){
	timeline_track_n3_add_indicator_n3(e, __indicator_n3__(n3));
	return 1;
      }
    }
  }
  
  return 0;
}

static void _roc_reset_(struct indicator *i)
{
  struct roc *ctx = (void*)i;
  average_reset(&ctx->average);
}

void roc_reset(struct roc *ctx)
{  
  _roc_reset_(__indicator__(ctx));
}

int roc_compute(struct roc *ctx, struct timeline_track_n3 *e,
                double *rvalue)
{
  struct timeline_track_n3 *ref = __list_relative__(e, -(ctx->period));
  if(ref != NULL){
    /* ROC formula :
     * ((candle[n] / candle[n - period]) - 1) * 100.0
     */
    double value = ((e->close / ref->close) - 1) * 100.0;
    double average = average_update(&ctx->average, value);
    
    if(average_is_available(&ctx->average)){
      *rvalue = average;
      return 0;
    }
  }
  
  return -1;
}

int roc_init(struct roc *ctx, unique_id_t uid, int period, int average)
{
  /* Super() */
  __indicator_init__(ctx, uid, roc_feed, _roc_reset_);
  __indicator_set_string__(ctx, "roc[%d,%d]", period, average);
  
  ctx->period = period;
  average_init(&ctx->average, AVERAGE_EXP, average);
  return 0;
}

void roc_release(struct roc *ctx)
{
  __indicator_release__(ctx);
  average_release(&ctx->average);
}
