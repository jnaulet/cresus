/*
 * Cresus EVO - updown.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/10/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "updown.h"
#include <string.h>

static int updown_feed(struct indicator *i, struct track_n3 *e)
{  
  struct updown *ctx = (void*)i;
  struct candle *c0 = (void*)e;
  
  /* FIXME : what if s1 == s2 or s0 == s1 ? */
  struct candle *c1 = __list_prev_n__(c0, 1);
  struct candle *c2 = __list_prev_n__(c1, 1);
  
  /* No list head */
  if(!__list_is_head__(c2) &&
     !__list_is_head__(c1)){
    
    /* Real values */
    double v2 = candle_get_value(c2, ctx->cvalue);
    double v1 = candle_get_value(c1, ctx->cvalue);
    double v0 = candle_get_value(c0, ctx->cvalue);
    
    /* Detect type */
    if(v0 < v1 && v1 > v2){
      /* UPDOWN_TOP */
      ctx->type = UPDOWN_TOP;
    }
    
    if(v0 > v1 && v1 < v2){
      /* UPDOWN_BOTTOM */
      ctx->type = UPDOWN_BOTTOM;
    }
  }
  
  return 0;
}

static void updown_reset(struct indicator *i)
{
  struct updown *ctx = (void*)i;
  /* RAZ */
}

int updown_init(struct updown *ctx, unique_id_t id, candle_value_t cvalue)
{  
  /* Super() */
  __indicator_init__(ctx, id, updown_feed, updown_reset);
  __indicator_set_string__(ctx, "updown");
  
  ctx->type = UPDOWN_NONE;
  ctx->cvalue = cvalue;
  
  return 0;
}

void updown_release(struct updown *ctx)
{
  __indicator_release__(ctx);
}
