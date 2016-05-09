/*
 * Cresus EVO - engine.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 09/01/2017
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include "engine.h"

int engine_init(struct engine *ctx, struct timeline *t) {

  ctx->timeline = t;
  /* Init lists */
  list_head_init(&ctx->list_order);
  list_head_init(&ctx->list_position_to_open);
  list_head_init(&ctx->list_position_opened);
  list_head_init(&ctx->list_position_to_close);
  list_head_init(&ctx->list_position_closed);
  
  return 0;
}

void engine_release(struct engine *ctx) {
  /* Nothing to do */
  list_head_release(&ctx->list_order);
  list_head_release(&ctx->list_position_to_open);
  list_head_release(&ctx->list_position_opened);
  list_head_release(&ctx->list_position_to_close);
  list_head_release(&ctx->list_position_closed);
}

static void engine_xfer_positions(struct engine *ctx,
				  list_head_t(struct position) *dst,
				  list_head_t(struct position) *src,
				  position_action_t position_action) {
  
  struct list *safe;
  struct position *p;
  
  __list_for_each_safe__(src, p, safe){
    __list_del__(p);
    /* Insert in dst */
    __list_add_tail__(dst, p);
    /* Act on position */
    position_action(p);
  }
}

static void engine_run_order(struct engine *ctx, struct order *o,
			     struct timeline_entry *e)
{
  struct candle *c = __timeline_entry_self__(e);
  
  switch(o->type){
  case ORDER_BUY:
    if(o->by == ORDER_BY_NB){
      /* Buy n positions */
      ctx->npos += o->value;
      ctx->amount -= (o->value * c->open);
    }else{
      /* Buy for x$ of positions */
      ctx->amount -= o->value;
      ctx->npos += (o->value / c->open);
    }
    break;
    
  default:
    break;
  }
}

void engine_run(struct engine *ctx, engine_feed_ptr feed) {

  struct list *safe;
  struct order *order;
  struct timeline_entry *entry;
  
  while((entry = timeline_step(ctx->timeline)) != NULL){
    /* First : check if there are opening positions */
    __list_for_each_safe__(&ctx->list_order, order, safe){
      engine_run_order(ctx, order, entry);
      __list_del__(order);
    }
    
    /* Then : feed the engine */
    feed(ctx, ctx->timeline, entry);
  }
}

int engine_place_order(struct engine *ctx, order_t type, order_by_t by,
		       double value)
{
  struct order *order;
  if(order_alloc(order, type, by, value)){
    __list_add_tail__(&ctx->list_order, order);
    return 0;
  }

  return -1;
}
