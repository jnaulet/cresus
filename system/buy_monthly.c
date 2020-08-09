/*
 * Cresus EVO - buy_monthly.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 25/02/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 *
 * Development sample code
 * Used to determine if interfaces are coherent for the "user"
 * And maybe to implement some kind of script commands 
 *
 */

#include <stdio.h>
#include <math.h>

#include "engine/engine_v2.h"

#include "framework/timeline_v2.h"
#include "framework/verbose.h"
#include "framework/quotes.h"

static int occurrence = 1;
static int check_income = 0;
static int check_debt = 0;

struct buy_monthly {
  int last_month;
};

static int buy_monthly_init(struct buy_monthly *ctx)
{
  ctx->last_month = -1;
  return 0;
}

#define buy_monthly_alloc(ctx)					\
  DEFINE_ALLOC(struct buy_monthly, ctx, buy_monthly_init)

static int check_total_revenue(struct buy_monthly *ctx,
                               struct track_n3 *track_n3)
{
  int i = 0;
  struct income_statement_n3 *p;
  struct income_statement_n3 *c = track_n3->income_statement.yearly;
  if(!c) return 0;
  
  while(++i){
    /* Get previous */
    p = (void*)list_prev_safe(&c->list);
    if(c->total_revenue <= p->total_revenue)
      return i;
    
    /* Get previous income statement */
    c = (void*)list_prev_safe(&c->list);
  }
}

static int check_net_debt(struct buy_monthly *ctx,
			  struct track_n3 *track_n3)
{
  struct balance_sheet_n3 *b = track_n3->balance_sheet.yearly;
  if(!b) return 0;
  
  return (b->short_term_debt + b->long_term_debt -
	  b->cash_and_short_term_investments);
}
                           
/* For each track */
static void feed_track_n3(struct engine_v2 *engine,
                          struct slice *slice,
                          struct track_n3 *track_n3)
{
  int order_anyway = 1;
  unique_id_t uid = track_n3->track->uid;
  int month = TIME64_GET_MONTH(slice->time);
  struct buy_monthly *ctx = track_n3->track->private;

  if(month != ctx->last_month && !(month % occurrence)){
    //PR_WARN("%s - BUY %d\n", track_n3_str(track_n3), amount);
    struct engine_v2_order *order;
    double amount = track_get_amount(track_n3->track, 500.0);
    
    /* Check fundamentals here */
    if(check_income){
      int n = check_total_revenue(ctx, track_n3);
      if(n < check_income){
        PR_ERR("%s: total revenue is trash\n", time64_str(track_n3->time, GR_DAY));
	order_anyway = 0;
      }
    }

    if(check_debt){
      if(check_net_debt(ctx, track_n3) > 0){
	PR_ERR("%s: net debt is trash\n", time64_str(track_n3->time, GR_DAY));
	order_anyway = 0;
      }
    }

    if(order_anyway){
      /* Send order */
      engine_v2_order_alloc(order, uid, BUY, amount, CASH);
      engine_v2_set_order(engine, order);
    }
  }
  
  /* Update ctx */
  ctx->last_month = month;
}

static struct engine_v2_interface engine_itf = {
  .feed_track_n3 = feed_track_n3,
};

static void custom_opt(struct timeline_v2 *t, char *opt, char *optarg)
{
  if(!strcmp(opt, "--modulo")) occurrence = atoi(optarg);
  if(!strcmp(opt, "--check-income")) check_income = atoi(optarg);
  if(!strcmp(opt, "--check-debt")) check_debt = 1;
}

static void customize_track(struct timeline_v2 *t, struct track *track)
{
  /* Allocate object */
  struct buy_monthly *buy_monthly;
  track->private = buy_monthly_alloc(buy_monthly);
}

static struct timeline_v2_ex_interface timeline_itf = {
   .custom_opt = custom_opt,
   .customize_track = customize_track
};

int main(int argc, char **argv)
{
  VERBOSE_LEVEL(INFO);
  
  /*
   * Data
   */
  
  int c;
  struct engine_v2 engine;
  struct timeline_v2 timeline;
  
  /* Check arguments */
  __try__(argc < 3, usage);

  /* Timeline + engine */
  timeline_v2_init_ex(&timeline, argc, argv, &timeline_itf);
  engine_v2_init_ex(&engine, &timeline, argc, argv);

  /* Start engine */
  engine_v2_run(&engine, &engine_itf);
  
  /* Release engine & more */
  engine_v2_release(&engine);
  timeline_v2_release(&timeline);
  
  return 0;

 __catch__(usage):
  fprintf(stderr, "Usage: %s %s %s [--modulo m] [--check-income n]\n",
	  argv[0], timeline_v2_ex_args, engine_v2_init_ex_args);
  return -1;
}
