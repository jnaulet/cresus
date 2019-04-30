/*
 * Cresus CommonTL - engine_v2.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/05/2019
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "engine_v2.h"
#include "framework/verbose.h"

/*
 * Orders
 */

/* Real order struct */
struct engine_v2_order {
  __inherits_from__(struct list);
  unique_id_t track_uid;
  engine_v2_order_t type;
  engine_v2_order_by_t by;
  double value;
};

static int engine_v2_order_init(struct engine_v2_order *ctx,
				unique_id_t track_uid,
				engine_v2_order_t type,
				double value,
				engine_v2_order_by_t by)
{
  __list_init__(ctx); /* super() */
  ctx->track_uid = track_uid;
  ctx->type = type;
  ctx->value = value;
  ctx->by = by;
  return 0;
}

static void engine_v2_order_release(struct engine_v2_order *ctx)
{
  __list_release__(ctx);
}

#define engine_v2_order_alloc(ctx, track_uid, type, value, by)		\
  DEFINE_ALLOC(struct engine_v2_order, ctx,				\
	       engine_v2_order_init, track_uid, type, value, by)
#define engine_v2_order_free(ctx)		\
  DEFINE_FREE(ctx, engine_v2_order_release)

/* 
 * Positions
 */
struct engine_v2_position {
  __inherits_from__(struct slist_by_uid);
  double shares, spent, earned;
};

int engine_v2_position_init(struct engine_v2_position *ctx, unique_id_t uid)
{
  __slist_by_uid_init__(ctx, uid);
  ctx->shares = 0.0;
  ctx->spent = 0.0;
  ctx->earned = 0.0;
  return 0;
}

void engine_v2_position_release(struct engine_v2_position *ctx)
{
  __slist_by_uid_release__(ctx);
}

#define engine_v2_position_alloc(ctx, uid)				\
  DEFINE_ALLOC(struct engine_v2_position, ctx,				\
	       engine_v2_position_init, uid)
#define engine_v2_position_free(ctx)					\
  DEFINE_FREE(ctx, engine_v2_position_release)

#define engine_v2_position_uid(ctx)		\
  __slist_by_uid__(ctx)->uid

/*
 * Engine v2
 */

int engine_v2_init(struct engine_v2 *ctx, struct timeline *t)
{
  /* Timeline */
  ctx->timeline = t;
  /* Time */
  ctx->start_time = TIME64_MIN;
  ctx->end_time = TIME64_MAX;
  /* Fees */
  ctx->transaction_fee = 0;
  /* Output */
  ctx->csv_output = 0;

  /* Stats */
  ctx->spent = 0.0;
  ctx->earned = 0.0;
  ctx->fees = 0.0;
  ctx->last_slice = NULL;

  /* Init lists */
  list_head_init(&ctx->list_orders);
  slist_head_init(&ctx->slist_positions);
  
  /* Init positions slist */
  struct timeline_track *track;
  __slist_for_each__(&t->by_track, track){
    struct engine_v2_position *p;
    engine_v2_position_alloc(p, timeline_track_uid(track));
    __slist_push__(&ctx->slist_positions, p);
  }
  
  return 0;
}

#define engine_v2_perf_pcent(assets, spent, earned)	\
  ((assets / (spent - earned) - 1.0) * 100.0)

static void engine_v2_display_stats(struct engine_v2 *ctx)
{
  struct engine_v2_position *p;
  double total_value = 0.0;
  double spent = 0.0, earned = 0.0;

  /* Assets */
  __slist_for_each__(&ctx->slist_positions, p){
    struct timeline_track_n3 *track_n3 =
      timeline_slice_get_track_n3(ctx->last_slice,
				  engine_v2_position_uid(p));

    double assets_value = track_n3->close * p->shares;
    total_value += assets_value;
    spent += p->spent;
    earned += p->earned;
    /* Display stats & performance */
    PR_STAT("%s assets value %.2lf, spent %.2lf, performance %.2lf%%\n",
	    track_n3->track->name, assets_value, p->spent,
	    engine_v2_perf_pcent(assets_value, p->spent, p->earned));
  }

  /* Total */
  PR_STAT("Total assets value %.2lf, spent %.2lf, performance %.2lf%%\n",
	  total_value, spent,
	  engine_v2_perf_pcent(total_value, spent, earned));
}

void engine_v2_release(struct engine_v2 *ctx)
{
  /* Display stats */
  engine_v2_display_stats(ctx);
  
  /* Clean positions slist */
  struct engine_v2_position *p;
  while(__slist_pop__(&ctx->slist_positions, &p) != NULL)
    engine_v2_position_free(p);
}

int engine_v2_set_common_opt(struct engine_v2 *ctx,
			     struct common_opt *opt)
{
  if(opt->start_time.set) ctx->start_time = opt->start_time.t;
  if(opt->end_time.set) ctx->end_time = opt->end_time.t;
  if(opt->transaction_fee.set) ctx->transaction_fee = opt->transaction_fee.d;
  if(opt->csv_output.set) ctx->csv_output = opt->csv_output.i;
  
  return 0;
}

static void engine_v2_buy_cash(struct engine_v2 *ctx,
			       struct engine_v2_position *p,
			       struct timeline_track_n3 *track_n3,
			       double value)
{
  double n = value / track_n3->open;
  p->shares += n;

  /* Stats */
  p->spent += value;
  ctx->fees += ctx->transaction_fee;
  
  PR_INFO("%s - Buy %.4lf securities for %.2lf CASH\n",
	  timeline_track_n3_str(track_n3), n, value);
}

static void engine_v2_sell_cash(struct engine_v2 *ctx,
				struct engine_v2_position *p,
				struct timeline_track_n3 *track_n3,
				double value)
{
  double n = value / track_n3->open;
  n = MIN(n, p->shares);
  p->shares -= n;

  /* Stats */
  p->earned += n * track_n3->open;
  ctx->fees += ctx->transaction_fee;
  
  PR_INFO("%s - Sell %.4lf securities for %.2lf CASH\n",
	  timeline_track_n3_str(track_n3), n, value);
}

static void engine_v2_run_orders(struct engine_v2 *ctx,
				 struct timeline_track_n3 *track_n3)
{
  struct engine_v2_order *order, *safe;
  __list_for_each_safe__(&ctx->list_orders, order, safe){
    /* Filter orders */
    if(TIME64CMP(track_n3->slice->time, ctx->start_time, GR_DAY) < 0)
      goto next;
    
    /* Ignore non-relevant orders
     * (order might stay until data is available) */
    if(order->track_uid != timeline_track_n3_track_uid(track_n3))
      continue;
    
    /* Find track-related position */
    struct engine_v2_position *p = (struct engine_v2_position*)
      __slist_by_uid_find__(&ctx->slist_positions, order->track_uid);

    /* Run order */
    switch(order->type){
    case BUY: engine_v2_buy_cash(ctx, p, track_n3, order->value); break;
    case SELL: break;
    }

  next:
    /* Remove executed order */
    __list_del__(order);
    engine_v2_order_free(order);
  }
}

void engine_v2_run(struct engine_v2 *ctx, struct engine_v2_interface *i)
{
  struct timeline_slice *slice;
  struct indicator_n3 *indicator_n3;
  struct timeline_track_n3 *track_n3;

  /* Run all slices */
  __list_for_each__(&ctx->timeline->by_slice, slice){
    
    /* We MUST stop at end_time */
    if(TIME64CMP(slice->time, ctx->end_time, GR_DAY) > 0)
      break;
    
    /* Run "new" slice */
    if(i->feed_slice)
      i->feed_slice(ctx, slice);
    
    /* Run "new" track */
    timeline_slice_for_each_track_n3(slice, track_n3){
      /* Run pending orders */
      engine_v2_run_orders(ctx, track_n3);
      
      if(i->feed_track_n3)
        i->feed_track_n3(ctx, slice, track_n3);
      
      /* Run "new" indicators */
      timeline_track_n3_for_each_indicator_n3(track_n3, indicator_n3){
        if(i->feed_indicator_n3)
          i->feed_indicator_n3(ctx, track_n3, indicator_n3);
      }
    }
    
    /* Post-processing */
    if(i->post_slice)
      i->post_slice(ctx, slice);

    /* Remember last slice for stats */
    ctx->last_slice = slice;
  }
}

int engine_v2_set_order(struct engine_v2 *ctx,
			struct timeline_track *track,
			engine_v2_order_t type, double value,
			engine_v2_order_by_t by)
{
  struct engine_v2_order *order;
  engine_v2_order_alloc(order, timeline_track_uid(track),
			type, value, by);
  if(!order) return -1;
  
  __list_add__(&ctx->list_orders, order);
  return 0;
}
