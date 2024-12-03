/*
 * Cresus EVO - bruss.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 03/11/2024
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bruss.h"

static int bruss_feed(struct indicator *i, struct track_n3 *e)
{  
  struct bruss_n3 *n3;
  struct bruss *ctx = (void*)i;

  int end = ctx->period - ctx->sample;

  for(int i = period; i > end; i--){
    struct track_n3 *p = track_n3_prev(e, i);
    
  }
  
  return 0;
}

static void bruss_reset(struct indicator *i)
{
  struct bruss *ctx = (void*)i;
  /* Nothing to do */
}

int bruss_init(struct bruss *ctx, unique_id_t uid, int period)
{  
  /* Super */
  indicator_init(&ctx->indicator, uid, bruss_feed, bruss_reset);
  indicator_set_string(&ctx->indicator, "bruss[%d]", period);
  
  ctx->period = period;
  ctx->sample = (int)((double)period / 2.71828);
  
  return 0;
}

void bruss_release(struct bruss *ctx)
{
  indicator_release(&ctx->indicator);
}
