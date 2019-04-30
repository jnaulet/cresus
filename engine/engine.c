/*
 * Cresus EVO - engine.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 09/01/2017
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>

#include "engine.h"
#include "framework/verbose.h"

int engine_init(struct engine *ctx, struct timeline *t)
{
  ctx->timeline = t;
  /* Init portfolio */
  list_head_init(&ctx->list_position);
  /* Money */
  ctx->npos_buy = 0;
  ctx->npos_sell = 0;
  ctx->amount = 0;
  ctx->earnings = 0;
  ctx->fees = 0;
  ctx->balance = 0;
  /* Stats */
  ctx->nbuy = 0;
  ctx->nsell = 0;
  ctx->max_drawdown = 0;
  ctx->transaction_fee = 0;
  /* Time boundaries */
  ctx->start_time = TIME64_MIN;
  ctx->end_time = TIME64_MAX;
  /* Misc */
  ctx->quiet = 0;
  /* Csv output */
  ctx->csv_output = 0;
  ctx->csv_ref = 0.0;
  
  return 0;
}

void engine_release(struct engine *ctx)
{
  /* Nothing to do */
  list_head_release(&ctx->list_position);
}

int engine_set_common_opt(struct engine *e, struct common_opt *opt)
{
  if(opt->start_time.set) e->start_time = opt->start_time.t;
  if(opt->end_time.set) e->end_time = opt->end_time.t;
  if(opt->transaction_fee.set) e->transaction_fee = opt->transaction_fee.d;
  if(opt->csv_output.set) e->csv_output = opt->csv_output.i;
  
  return 0;
}

double engine_npos(struct engine *ctx, int *nrec)
{
  int n = 0;
  double ret = 0.0;
  struct position *p;
  
  __list_for_each__(&ctx->list_position, p){
    if(p->status == POSITION_CONFIRMED){
      ret = ((p->type == BUY) ? (ret + p->n) : (ret - p->n));
      n++;
    }
  }

  if(nrec) *nrec = n;
  return ret;
}

double engine_assets_value(struct engine *ctx, double close)
{
  double ret = 0.0;
  struct position *p;
  
  __list_for_each__(&ctx->list_position, p){
    if(p->status != POSITION_DESTROY)
      ret += position_current_value(p, close);
  }

  return ret;
}

double engine_assets_original_value(struct engine *ctx)
{
  double ret = 0.0;
  struct position *p;
  
  __list_for_each__(&ctx->list_position, p){
    if(p->status == POSITION_CONFIRMED)
      ret += (p->n * p->value);
  }

  return ret;
}

static void engine_run_csv_output(struct engine *ctx,
                                  struct timeline_slice *slice)
{
#if 0
  if(TIME64CMP(slice->time, ctx->start_time, GR_DAY) >= 0){
    struct timeline_slice_n3 *n3;
    __slist_for_each__(slice->slist_n3s, n3){
      struct timeline_track_n3 *c = n3->track_n3;
      double orig = engine_assets_original_value(ctx);
      double value = engine_assets_value(ctx, c->close);
      
      if(ctx->csv_ref == 0.0)
        ctx->csv_ref = c->close; /* One-time init */
      
      else{
        double ref = (c->close / ctx->csv_ref)  - 1.0;
        double target = (orig != 0.0 ? (value / orig) - 1.0 : 0.0);
        /* Output csv */
        printf("%s, %lf, %lf, %lf\n", timeline_n3_str(e),
               ref , target, target - ref);
      }
    }
  }
#endif
}

static void engine_run_position_buy(struct engine *ctx,
				    struct position *p,
				    struct timeline_track_n3 *c)
{
  /* Record open value */
  position_set_value(p, c->open);
  
  if(p->req == SHARES){
    double unit = position_unit_value(p);
    position_set_n(p, p->request.shares);
    PR_INFO("%s - Buy %.0lf securities at %.2lf (%.2lf) VALUE\n",
	    timeline_track_n3_str(c),
            p->n * 1.0, unit, p->n * unit);
    
    /* Some stats */
    ctx->amount += p->n * unit;
    ctx->balance -= p->n * unit;
    
  }else{
    position_set_n(p, p->request.cash / c->open);
    PR_INFO("%s - Buy %.4lf securities for %.2lf CASH\n",
            timeline_track_n3_str(c),
            p->n * 1.0, p->request.cash);

    /* Some stats */
    ctx->amount += p->request.cash;
    ctx->balance -= p->request.cash;
  }
  
  /* Confirm position */
  position_confirm(p);
}

static void engine_run_position_sell(struct engine *ctx,
				     struct position *p,
				     struct timeline_track_n3 *c)
{
  /* Stats */
  double req = 0.0;
  double cash = 0.0;
  double nsold = 0.0;
  struct position *lp;
  
  if(p->req == SHARES) req = p->request.shares;
  else req = p->request.cash / c->open;
  
  __list_for_each_prev__(&ctx->list_position, lp){
    double count;
    /* No stopped or req positions */
    if(lp->status != POSITION_CONFIRMED)
      continue;
    
    /* Output condition */
    if(req <= 0.0)
      break;

    /* How much at once ? */
    count = MIN(req, lp->n);
    /* Update counters */
    req -= count;
    lp->n -= count;
    nsold += count;
    /* Beware of short positions. FIXME */
    cash += count * ((c->open - lp->cert.funding) / lp->cert.ratio);
    
    /* Mark position for destruction if 0 */
    if(lp->n <= 0.0)
      position_destroy(lp);
  }

  /* Display info */
  if(nsold > 0.0){
    if(p->req == SHARES){
      PR_WARN("%s - Sell %.0lf securities at %.2lf (%.2lf) VALUE\n",
	      timeline_track_n3_str(c),
              nsold, cash / nsold, cash);
    }else{
      PR_WARN("%s - Sell %.4lf securities for %.2lf CASH\n",
              timeline_track_n3_str(c),
              nsold, cash);
    }
  }
  
  /* The end */
  ctx->earnings += cash;
  ctx->balance += cash;
  /* Don't forget to mark position */
  position_destroy(p);
}

static void engine_run_position(struct engine *ctx,
				struct position *p,
				struct timeline_track_n3 *c)
{
  switch(p->type){
  case BUY: engine_run_position_buy(ctx, p, c); break;
  case SELL: engine_run_position_sell(ctx, p, c); break;
  default: PR_WARN("Not implemented\n"); return;
  }
  
  ctx->fees += ctx->transaction_fee;
  ctx->max_drawdown = MIN(ctx->balance, ctx->max_drawdown);
}

void engine_run(struct engine *ctx, engine_feed_ptr feed)
{
  struct list *safe;
  struct position *p;
  struct timeline_slice *slice;
  
  /* Parse by slice */
  __list_for_each__(&ctx->timeline->by_slice, slice){
    
    /* We MUST stop at end_time */
    if(TIME64CMP(slice->time, ctx->end_time, GR_DAY) > 0)
      break;

    /* Positions management */
    __list_for_each_safe__(&ctx->list_position, p, safe){
      /* 1st : check if there are opening positions */
      if(p->status == POSITION_REQUESTED)
	engine_run_position(ctx, p, NULL); /* Run */
      
      /* 2nd: check stoplosses */
      if(c->low <= p->cert.stoploss){
	p->status = POSITION_DESTROY;
	PR_WARN("%s - Stoploss hit\n", timeline_track_n3_str(c));
      }
      
      /* 3rd: Remove useless positions (sales & lost buys) */
      if(p->status == POSITION_DESTROY){
	__list_del__(p);
	position_free(p);
      }
    }
    
    /* Then : feed the engine */
    feed(ctx, ctx->timeline, slice);
    ctx->close = c->close;

    /* In the end : output csv if asked for */
    if(ctx->csv_output)
      engine_run_csv_output(ctx, slice);
  }
}

int engine_set_order(struct engine *ctx, position_t type,
		     double value, position_req_t req, 
		     struct cert *cert)
{
  struct position *p;
  struct timeline_n3 *n3;
  
  if(timeline_n3_current(ctx->timeline, &n3) != -1){
    /* Filter orders if needed */
    if(TIME64CMP(n3->time, ctx->start_time, GR_DAY) < 0)
      goto err;
    
    if(position_alloc(p, type, req, value, cert)){
      __list_add_tail__(&ctx->list_position, p);
      return 0;
    }
  }
  
 err:
  return -1;
}

int engine_place_order(struct engine *ctx, position_t type,
		       position_req_t by, double value)
{
  struct timeline_n3 *n3;
  return engine_set_order(ctx, type, by, value, NULL);
}

void engine_get_stats(struct engine *ctx, struct engine_stats *stats)
{
  /* Basics */
  stats->amount = ctx->amount;
  stats->earnings = ctx->earnings;
  stats->fees = ctx->fees;
  /* What we got left */
  stats->assets_value = engine_assets_value(ctx, ctx->close);
  stats->total_value = stats->assets_value + ctx->earnings - ctx->fees;
  /* Return on investment */
  stats->roi = ((stats->total_value / ctx->amount) - 1.0) * 100.0;
  /* More info */
  stats->balance = ctx->balance;
  stats->max_drawdown = ctx->max_drawdown;
  stats->rrr = (ctx->balance / fabs(ctx->max_drawdown)) * 100.0;
}

void engine_display_stats_r(struct engine *ctx, struct engine_stats *stats)
{
  double npos = 0.0; /* ! */
  
  /* Basic informations */
  PR_STAT("amount: %.2lf, earnings: %.2lf, npos: %.2lf, fees: %.2lf\n",
	  stats->amount, stats->earnings, npos, stats->fees);
  /* More stats */
  //PR_ERR("nbuy: %d, nsell: %d ", ctx->nbuy, ctx->nsell);
  /* Values */
  PR_STAT("assets_value: %.2lf, total_value: %.2lf, roi: %.2lf%%\n",
	  stats->assets_value, stats->total_value, stats->roi);
  /* Interesting stuff */
  PR_STAT("balance: %.2lf, max_drawdown: %.2lf, rrr: %.2lf\n",
	  stats->balance, stats->max_drawdown, stats->rrr);
}

void engine_display_stats(struct engine *ctx)
{
  struct engine_stats stats;
  engine_get_stats(ctx, &stats);
  engine_display_stats_r(ctx, &stats);
}
