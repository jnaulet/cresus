/*
 * Cresus EVO - hilo.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 12.25.2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */
#include "hilo.h"

static void _hilo_reset_(struct indicator *i)
{
  struct hilo *ctx =  (void*)i;
  // TODO
}

void hilo_reset(struct hilo *ctx)
{
  _hilo_reset_(__indicator__(ctx));
}

#define HIGH(x, y) (((x) > (y)) ? (x) : (y))
#define LOW(x, y) (((x) < (y)) ? (x) : (y))

static int hilo_feed(struct indicator *i, struct timeline_track_n3 *e)
{
  struct hilo_n3 *n3;
  struct hilo *ctx = (void*)i;
  struct timeline_track_n3 *prev = NULL;
  
  if(hilo_n3_alloc(n3, i)){
    /* Init */
    int n = ctx->period;
    n3->high = e->high;
    n3->low = e->low;
    
    __list_for_each_prev__(e, prev){
      
      /* Out after ctx->period iterations */
      if(!--n)
	break;
      
      n3->high = HIGH(n3->high, prev->high);
      n3->low = LOW(n3->low, prev->low);
    }
    
    if(ctx->filter && n)
      goto out;
    
    /* Attach new n3 */
    timeline_track_n3_add_indicator_n3(e, __indicator_n3__(n3));
    return 1;
  }

 out:
  return 0;
}

int hilo_init(struct hilo *ctx, unique_id_t id, int period, int filter)
{
  /* Super() */
  __indicator_init__(ctx, id, hilo_feed, _hilo_reset_);
  __indicator_set_string__(ctx, "hilo[%d]", period);
  
  ctx->period = period;
  ctx->filter = filter; /* bool to filter incomplete data */
  return 0;
}

void hilo_release(struct hilo *ctx)
{
  __indicator_release__(ctx);
}
