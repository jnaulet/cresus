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
  struct hilo *ctx =  (struct hilo*)i;
  // TODO
}

void hilo_reset(struct hilo *ctx)
{
  _hilo_reset_(&ctx->indicator);
}

#define HIGH(x, y) (((x) > (y)) ? (x) : (y))
#define LOW(x, y) (((x) < (y)) ? (x) : (y))

static int hilo_feed(struct indicator *i, struct track_n3 *e)
{
  struct list *l;
  struct hilo_n3 *n3;
  struct price_n3 *price = e->price;
  struct hilo *ctx = (struct hilo*)i;
  
  if(hilo_n3_alloc(n3, i)){
    /* Init */
    int n = ctx->period;
    n3->high = price->high;
    n3->low = price->low;
    
    list_for_each_prev(&price->list, l){
      struct price_n3 *prev = (void*)l;
      
      /* Out after ctx->period iterations */
      if(!--n)
	break;
      
      n3->high = HIGH(n3->high, prev->high);
      n3->low = LOW(n3->low, prev->low);
    }
    
    if(ctx->filter && n)
      goto out;
    
    /* Attach new n3 */
    track_n3_add_indicator_n3(e, &n3->indicator_n3);
    return 1;
  }
  
 out:
  return 0;
}

int hilo_init(struct hilo *ctx, unique_id_t id, int period, int filter)
{
  /* Super() */
  indicator_init(&ctx->indicator, id, hilo_feed, _hilo_reset_);
  indicator_set_string(&ctx->indicator, "hilo[%d]", period);
  
  ctx->period = period;
  ctx->filter = filter; /* bool to filter incomplete data */
  return 0;
}

void hilo_release(struct hilo *ctx)
{
  indicator_release(&ctx->indicator);
}
