/*
 * Cresus EVO - srsi.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdlib.h>
#include "srsi.h"
#include "engine/candle.h"

static int srsi_feed(struct indicator *i, struct timeline_track_n3 *e)
{  
  struct srsi *ctx = (void*)i;
  struct candle *c = (void*)e;
  int start = (c->open < c->close ? c->open : c->close);
  int end = (c->close < c->open ? c->open : c->close);
  
  for(int i = start; i <= end; i++){
    if(c->close >= c->open)
      ctx->array[i].bull++;
    else
      ctx->array[i].bear++;
    
    ctx->array[i].total++;
  }
  
  ctx->len++;
  return 0;
}

static void srsi_reset(struct indicator *i) {

  struct srsi *ctx = (void*)i;
  ctx->len = 0;
}

int srsi_init(struct srsi *ctx, unique_id_t id, int max) {
  
  /* init() */
  __indicator_init__(ctx, id, srsi_feed, srsi_reset);
  __indicator_set_string__(ctx, "srsi[%d]", max);
  
  ctx->len = 0;
  ctx->max = max;
  
  if((ctx->array = malloc(sizeof(*ctx->array) * max)))
    return -1;
  
  return 0;
}

void srsi_release(struct srsi *ctx) {
  
  __indicator_release__(ctx);
  free(ctx->array);
}
