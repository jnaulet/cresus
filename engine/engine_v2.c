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

int engine_v2_init(struct engine_v2 *ctx, struct timeline_v2 *t)
{
  /* Timeline */
  ctx->timeline_v2 = t;
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
  plist_head_init(&ctx->plist_orders);

  /* Portfolio */
  portfolio_init(&ctx->portfolio);
  
  return 0;
}

int engine_v2_init_ex(struct engine_v2 *ctx, struct timeline_v2 *t,
		      int argc, char **argv)
{
  /* Basics */
  engine_v2_init(ctx, t);

  for(int i = 0; i < argc; i++){
    /* Csv output */
    if(!strcmp(argv[i], "--csv")){
      ctx->csv_output = open(argv[++i], O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR);
      dprintf(ctx->csv_output, "date, invested, spent, earned, gain/loss, index\n");
    }
    /* Start */
    if(!strcmp(argv[i], "--start")){
      ctx->start_time = time64_atot(argv[++i]);
    }
    /* Stop/end */
    if(!strcmp(argv[i], "--stop") ||
       !strcmp(argv[i], "--end")){
      ctx->end_time = time64_atot(argv[++i]);
    }
    /* Verbosity */
    if(!strcmp(argv[i], "--verbose") ||
       !strcmp(argv[i], "--debug")){
      VERBOSE_LEVEL(DBG);
    }
  }
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
		      struct slice *slice)
{
  struct plist *p;
  double total_value = 0.0;
  
  /* Portfolio */
  plist_for_each(&ctx->portfolio.plist_portfolio_n3s, p){
    struct portfolio_n3 *pos = p->ptr;
    /* Find track_n3 by uid */
    struct quotes_n3 *quotes_n3 =
      slice_get_track_n3(slice, pos->uid)->quotes;
    
    /* Compute total value */
    total_value += portfolio_n3_total_value(pos, quotes_n3->close);
  }
  
  return total_value;
}

static void engine_v2_csv_output(struct engine_v2 *ctx,
				 struct slice *slice)
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
  struct plist *p;
  double total_value = 0.0;
  
  /* Portfolio */
  
  plist_for_each(&ctx->portfolio.plist_portfolio_n3s, p){
    struct portfolio_n3 *pos = p->ptr;
    /* Find track name by uid */
    struct track *track =
      timeline_v2_find_track(ctx->timeline_v2, pos->uid);
    /* Get last track n3 */
    struct track_n3 *track_n3 =
      track->plist_track_n3s.prev->ptr;
    
    /* Display track stats */
    portfolio_n3_pr_stat(pos, track_n3->quotes->close);
    total_value += portfolio_n3_total_value(pos, track_n3->quotes->close);
  }
  
  /* Total */
  engine_v2_print_stats("Total", total_value, ctx->spent,
			ctx->earned, ctx->fees);
}

static void
engine_v2_display_pending_orders(struct engine_v2 *ctx)
{
  struct plist *p;
  
  plist_for_each(&ctx->plist_orders, p){
    struct engine_v2_order *order = p->ptr;
    /* Find track name by uid */
    struct track *track =
      timeline_v2_find_track_by_uid(ctx->timeline_v2, order->track_uid);
    /* Get last track n3 */
    struct track_n3 *track_n3 =
      track->plist_track_n3s.prev->ptr;
    
    fprintf(stdout, "%s %.2lf %s %.2lf %d\n",
	    track->name, track_n3->quotes->close,
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

static void engine_v2_buy_cash(struct engine_v2 *ctx,
			       struct track_n3 *track_n3,
			       struct engine_v2_order *order)
{
  /* Convert CASH to shares */
  double shares = engine_v2_order_shares(order,
					 track_n3->quotes->open);
  
  /* Portfolio */
  portfolio_add(&ctx->portfolio,
		track_n3->track->name, order->track_uid,
		shares, track_n3->quotes->open);
  
  /* Stats */
  ctx->spent += order->value;
  ctx->fees += track_n3->track->transaction_fee;
  
  PR_INFO("%s - Buy %.4lf securities for %.2lf CASH\n",
	  track_n3_str(track_n3), shares, order->value);
}

static void engine_v2_sell_cash(struct engine_v2 *ctx,
				struct track_n3 *track_n3,
				struct engine_v2_order *order)
{
  /* Convert CASH to shares */
  double shares = engine_v2_order_shares(order,
					 track_n3->quotes->open);
  
  /* Portfolio */
  order->value = portfolio_sub(&ctx->portfolio,
			       track_n3->track->name,
			       order->track_uid, shares,
			       track_n3->quotes->open);
  
  /* Stats */
  ctx->earned += order->value;
  ctx->fees += track_n3->track->transaction_fee;
  
  PR_INFO("%s - Sell %.4lf securities for %.2lf CASH\n",
	  track_n3_str(track_n3), shares, order->value);
}

static void engine_v2_run_orders(struct engine_v2 *ctx,
				 struct track_n3 *track_n3)
{
  struct plist *p, *safe;
  
  plist_for_each_safe(&ctx->plist_orders, p, safe){
    struct engine_v2_order *order = p->ptr;
    /* Ignore non-relevant orders
     * (order might stay until data is available) */
    if(order->track_uid != track_n3->track->uid)
      continue;
    
    /* Run order */
    switch(order->type){
    case BUY: engine_v2_buy_cash(ctx, track_n3, order); break;
    case SELL: engine_v2_sell_cash(ctx, track_n3, order); break;
    }
    
    /* Remove executed order */
    plist_del(p);
    engine_v2_order_free(order);
  }
}

static void engine_run_before_start(struct engine_v2 *ctx,
				    struct slice *slice,
				    struct engine_v2_interface *i)
{
#if 0
  struct indicator_n3 *indicator_n3;
  struct track_n3 *track_n3;
  
  /* Run "new" track */
  slice_for_each_track_n3(slice, track_n3){
    /* Run "new" indicators */
    track_n3_for_each_indicator_n3(track_n3, indicator_n3){
      if(i->feed_indicator_n3)
	i->feed_indicator_n3(ctx, track_n3, indicator_n3);
    }
  }
#endif
}

static void engine_run_after_start(struct engine_v2 *ctx,
				   struct slice *slice,
				   struct engine_v2_interface *i)
{
  struct track_n3 *track_n3;
  struct indicator_n3 *indicator_n3;
  
  /* Run "new" slice */
  if(i->feed_slice)
    i->feed_slice(ctx, slice);
  
  /* Run "new" track */
  slice_for_each_track_n3(slice, track_n3){
    /* Run pending orders */
    engine_v2_run_orders(ctx, track_n3);

    /* Callback */
    if(i->feed_track_n3)
      i->feed_track_n3(ctx, slice, track_n3);
    
    /* Run "new" indicators */
    track_n3_for_each_indicator_n3(track_n3, indicator_n3){
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
  struct plist *p;

  /* Run all slices */
  plist_for_each(&ctx->timeline_v2->by_slice, p){
    struct slice *slice = p->ptr;
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
  plist_add_ptr(&ctx->plist_orders, order);
  return 0;
}
