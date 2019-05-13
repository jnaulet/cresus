/*
 * Cresus CommonTL - engine_v2.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/05/2019
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "engine_v2.h"
#include "framework/verbose.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
  /* Output */
  ctx->csv_output = -1;

  /* Stats */
  ctx->spent = 0.0;
  ctx->earned = 0.0;
  ctx->fees = 0.0;
  ctx->last_slice = NULL;

  /* Init lists */
  list_head_init(&ctx->list_orders);

  /* Portfolio */
  portfolio_init(&ctx->portfolio);
  
  return 0;
}

/*
 * Stats. TODO : put somewhere else
 */

#define engine_v2_performance_pcent(assets, spent, earned, fees)	\
  (((assets + earned) / (spent + fees) - 1.0) * 100.0)

#define engine_v2_print_stats(name, value, spent, earned, fees)		\
  PR_STAT("%s total %.2lf assets %.2lf spent %.2lf earned %.2lf "	\
	  "fees %.2lf performance %.2lf%%\n",				\
	  name, earned + value, value, spent, earned, fees,		\
	  engine_v2_performance_pcent(value, spent, earned, fees));

static double
engine_v2_total_value(struct engine_v2 *ctx,
		      struct timeline_slice *slice)
{
  double total_value = 0.0;
  
  /* Portfolio */
  struct portfolio_n3 *pos;
  __list_for_each__(&ctx->portfolio.list_portfolio_n3s, pos){
    /* Find track_n3 by uid */
    struct timeline_track_n3 *track_n3 =
      timeline_slice_get_track_n3(slice, pos->uid);
    
    /* Compute total value */
    total_value += portfolio_n3_total_value(pos, track_n3->close);
  }
  
  return total_value;
}

static void engine_v2_csv_output(struct engine_v2 *ctx,
				 struct timeline_slice *slice)
{
  double value = engine_v2_total_value(ctx, slice);
  double balance = (ctx->spent + ctx->earned);
  double gainloss = value - balance;
  double index = balance != 0.0 ? (gainloss / balance) : 0.0;
  
  dprintf(ctx->csv_output,
	  "%s, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf\n",
	  time64_str(slice->time, GR_DAY), value,
	  ctx->spent, ctx->earned, gainloss, index);
}

static void engine_v2_display_stats(struct engine_v2 *ctx)
{
  double total_value = 0.0;
  
  /* Portfolio */
  struct portfolio_n3 *pos;
  __list_for_each__(&ctx->portfolio.list_portfolio_n3s, pos){
    /* Find track name by uid */
    struct timeline_track *track =
      timeline_find_track(ctx->timeline, pos->uid);
    /* Get last track n3 */
    struct timeline_track_n3 *track_n3 = (void*)
      track->list_track_n3s.prev;
    
    /* Display track stats */
    portfolio_n3_pr_stat(pos, track_n3->close);
    total_value += portfolio_n3_total_value(pos, track_n3->close);
  }
  
  /* Total */
  engine_v2_print_stats("Total", total_value, ctx->spent,
			ctx->earned, ctx->fees);
}

static void
engine_v2_display_pending_orders(struct engine_v2 *ctx)
{
  struct engine_v2_order *order;
  __list_for_each__(&ctx->list_orders, order){
    /* Find track name by uid */
    struct timeline_track *track =
      timeline_find_track(ctx->timeline, order->track_uid);
    /* Get last track n3 */
    struct timeline_track_n3 *track_n3 = (void*)
      track->list_track_n3s.prev;
    
    fprintf(stdout, "%s %.2lf %s %.2lf %d\n",
	    track->name, track_n3->close,
	    (order->type == BUY ? "buy" : "sell"),
	    order->value, order->level);
  }
}

void engine_v2_release(struct engine_v2 *ctx)
{
  /* Display stats */
  engine_v2_display_stats(ctx);

  /* Display pending orders */
  engine_v2_display_pending_orders(ctx);
  
  /* Clean portfolio */
  portfolio_release(&ctx->portfolio);

  /* Close csv */
  if(ctx->csv_output != -1)
    close(ctx->csv_output);
}

int engine_v2_set_common_opt(struct engine_v2 *ctx,
			     struct common_opt *opt)
{
  if(opt->start_time.set) ctx->start_time = opt->start_time.t;
  if(opt->end_time.set) ctx->end_time = opt->end_time.t;
  if(opt->csv_output.set){
    if((ctx->csv_output = open(opt->csv_output.s,
			       O_WRONLY|O_CREAT|O_TRUNC,
			       S_IWUSR|S_IRUSR)) != -1)
      dprintf(ctx->csv_output,
	      "date, invested, spent, earned, gain/loss, index\n");
  }
  
  return 0;
}

static void engine_v2_buy_cash(struct engine_v2 *ctx,
			       struct timeline_track_n3 *track_n3,
			       struct engine_v2_order *order)
{
  /* Convert CASH to shares */
  double shares = engine_v2_order_shares(order, track_n3->open);
  
  /* Portfolio */
  if(order->funding > 0)
    portfolio_add_leveraged(&ctx->portfolio,
			    track_n3->track->name, order->track_uid,
			    shares, track_n3->open, order->funding,
			    order->ratio, order->stoploss);
  else
    portfolio_add(&ctx->portfolio,
		  track_n3->track->name, order->track_uid,
		  shares, track_n3->open);
  
  /* Stats */
  ctx->spent += order->value;
  ctx->fees += track_n3->track->transaction_fee;
  
  PR_INFO("%s - Buy %.4lf securities for %.2lf CASH\n",
	  timeline_track_n3_str(track_n3), shares, order->value);
}

static void engine_v2_sell_cash(struct engine_v2 *ctx,
				struct timeline_track_n3 *track_n3,
				struct engine_v2_order *order)
{
  /* Convert CASH to shares */
  double shares = engine_v2_order_shares(order, track_n3->open);
  
  /* Portfolio */
  order->value = portfolio_sub(&ctx->portfolio,
			       track_n3->track->name,
			       order->track_uid,
			       shares, track_n3->open);
  
  /* Stats */
  ctx->earned += order->value;
  ctx->fees += track_n3->track->transaction_fee;
  
  PR_INFO("%s - Sell %.4lf securities for %.2lf CASH\n",
	  timeline_track_n3_str(track_n3), shares, order->value);
}

static void engine_v2_run_orders(struct engine_v2 *ctx,
				 struct timeline_track_n3 *track_n3)
{
  struct engine_v2_order *order, *safe;
  __list_for_each_safe__(&ctx->list_orders, order, safe){
    /* Ignore non-relevant orders
     * (order might stay until data is available) */
    if(order->track_uid != timeline_track_n3_track_uid(track_n3))
      continue;
    
    /* Run order */
    switch(order->type){
    case BUY: engine_v2_buy_cash(ctx, track_n3, order); break;
    case SELL: engine_v2_sell_cash(ctx, track_n3, order); break;
    }
    
    /* Remove executed order */
    __list_del__(order);
    engine_v2_order_free(order);
  }
}

static void engine_run_before_start(struct engine_v2 *ctx,
				    struct timeline_slice *slice,
				    struct engine_v2_interface *i)
{
  struct indicator_n3 *indicator_n3;
  struct timeline_track_n3 *track_n3;
  
  /* Run "new" track */
  timeline_slice_for_each_track_n3(slice, track_n3){
    /* Run "new" indicators */
    timeline_track_n3_for_each_indicator_n3(track_n3, indicator_n3){
      if(i->feed_indicator_n3)
	i->feed_indicator_n3(ctx, track_n3, indicator_n3);
    }
  }
}

static void engine_run_after_start(struct engine_v2 *ctx,
				   struct timeline_slice *slice,
				   struct engine_v2_interface *i)
{
  struct indicator_n3 *indicator_n3;
  struct timeline_track_n3 *track_n3;
  
  /* Run "new" slice */
  if(i->feed_slice)
    i->feed_slice(ctx, slice);
  
  /* Run "new" track */
  timeline_slice_for_each_track_n3(slice, track_n3){
    /* Run pending orders */
    engine_v2_run_orders(ctx, track_n3);
    /* Run portfolio */
    portfolio_run(&ctx->portfolio, track_n3->low); /* FIXME */
    
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
  
  /* Csv output */
  if(ctx->csv_output != -1)
    engine_v2_csv_output(ctx, slice);
}

void engine_v2_run(struct engine_v2 *ctx,
		   struct engine_v2_interface *i)
{
  struct timeline_slice *slice;

  /* Run all slices */
  __list_for_each__(&ctx->timeline->by_slice, slice){    
    /* We MUST stop at end_time */
    if(TIME64CMP(slice->time, ctx->end_time, GR_DAY) > 0)
      break;

    /* Debug */
    PR_DBG("engine_v2.c: playing slice #%s\n",
	   time64_str(slice->time, GR_DAY));

    /* Run in two-state mode */
    if(TIME64CMP(slice->time, ctx->start_time, GR_DAY) < 0)
      engine_run_before_start(ctx, slice, i);
    else
      engine_run_after_start(ctx, slice, i);
    
    /* Remember last slice for stats */
    ctx->last_slice = slice;
  }
}

int engine_v2_set_order(struct engine_v2 *ctx,
			struct engine_v2_order *order)
{
  __list_add__(&ctx->list_orders, order);
  return 0;
}
