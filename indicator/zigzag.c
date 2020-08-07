/*
 * Cresus EVO - zigzag.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <string.h>

#include "zigzag.h"

static void zigzag_chdir(struct zigzag *ctx, zigzag_dir_t dir,
                         struct track_n3 *e)
{  
  ctx->base_ref = ctx->ref;
  ctx->dir = dir;
  ctx->ref = e;
  ctx->ref_count = 0;
}

static int zigzag_feed(struct indicator *i, struct track_n3 *e)
{  
  double threshold;
  struct zigzag_n3 *n3;
  struct zigzag *ctx = (void*)i;
  double value = quotes_n3_value(e->quotes, ctx->value);
  
  if(!ctx->ref){
    ctx->ref = e;
    zigzag_chdir(ctx, ZIGZAG_NONE, e);
    /* Nothing left to do */
    return 0;
  }
  
  /* Compute limits (every time ? */
  double base_ref_value = quotes_n3_value(ctx->base_ref->quotes, ctx->value);
  double ref_value = quotes_n3_value(ctx->ref->quotes, ctx->value);
  double hi_limit = (1.0 + ctx->threshold) * ref_value;
  double lo_limit = (1.0 - ctx->threshold) * ref_value;
  
  switch(ctx->dir){
  case ZIGZAG_NONE :
    if(value > hi_limit) zigzag_chdir(ctx, ZIGZAG_UP, e);
    if(value < lo_limit) zigzag_chdir(ctx, ZIGZAG_DOWN, e);
    break;
    
  case ZIGZAG_UP :
    if(value > ref_value) ctx->ref = e;
    if(value < lo_limit) zigzag_chdir(ctx, ZIGZAG_DOWN, e);
    break;
    
  case ZIGZAG_DOWN :
    if(value < ref_value) ctx->ref = e;
    if(value > hi_limit) zigzag_chdir(ctx, ZIGZAG_UP, e);
    break;
  }
  
  if(zigzag_n3_alloc(n3, i, ctx->dir, (value / base_ref_value), ctx->ref_count))
    track_n3_add_indicator_n3(e, &n3->indicator_n3);
  
  ctx->ref_count++;
  return 0;
}

static void zigzag_reset(struct indicator *i)
{
  struct zigzag *ctx = (void*)i;
  /* RAZ */
  ctx->dir = ZIGZAG_NONE;
  /* Refs */
  ctx->ref = NULL;
  ctx->base_ref = NULL;
  ctx->ref_count = 0;
}

int zigzag_init(struct zigzag *ctx, unique_id_t uid,
		double threshold, quotes_n3_value_t value)
{  
  /* Super */
  indicator_init(&ctx->indicator, uid, zigzag_feed, zigzag_reset);
  indicator_set_string(&ctx->indicator, "zigzag[%.1f%%]", threshold * 100.0);
  
  ctx->dir = ZIGZAG_NONE;
  ctx->value = value;
  ctx->threshold = threshold;
  
  /* Refs */
  ctx->ref = NULL;
  ctx->base_ref = NULL;
  ctx->ref_count = 0;
  
  return 0;
}

void zigzag_release(struct zigzag *ctx)
{
  indicator_release(&ctx->indicator);
  ctx->ref_count = 0;
  ctx->dir = ZIGZAG_NONE;
}
