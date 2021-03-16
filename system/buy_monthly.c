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

#include "indicator/rs_roc.h"
#include "indicator/metrics.h"

static int occurrence = 1;
static int metrics = 0;

static double pe = 0.0;
static double pbv = 0.0;
static double de = 0.0;
static double da = 0.0;

#define METRICS_UID 0
#define RS_UID      1

struct buy_monthly {
  int last_month;
  int metrics_ok;
};

static int buy_monthly_init(struct buy_monthly *ctx)
{
  ctx->last_month = -1;
  ctx->metrics_ok = 0;
  return 0;
}

#define buy_monthly_alloc(ctx)                                  \
  DEFINE_ALLOC(struct buy_monthly, ctx, buy_monthly_init)

/* For each track */
static void feed_track_n3(struct engine_v2 *engine,
                          struct slice *slice,
                          struct track_n3 *track_n3)
{
  int order_anyway = 1;
  unique_id_t uid = track_n3->track->uid;
  struct buy_monthly *ctx = track_n3->track->private;
  
  struct tm *tm = localtime(&slice->time);
  int month = tm->tm_mon + 1;

  /* Ignore ref track */
  if(!ctx) return;
  
  if(month != ctx->last_month && !(month % occurrence)){
    //PR_WARN("%s - BUY %d\n", track_n3_str(track_n3), amount);
    struct engine_v2_order *order;
    double amount = track_get_amount(track_n3->track, 500.0);

    /* Indicators */
    struct rs_roc_n3 *r = (void*)track_n3_get_indicator_n3(track_n3, RS_UID);
    struct metrics_n3 *m = (void*)track_n3_get_indicator_n3(track_n3, METRICS_UID);

    /* Check technical here */
    if(r && r->value <= 0.0){
      PR_WARN("Bad month: ignore order\n");
      order_anyway = 0;
    }
    
    /* Check fundamentals here */
    if(metrics && !ctx->metrics_ok){
      PR_DBG("%s: %s metrics are trash\n",
             time_to_iso8601(track_n3->time),
             track_n3->track->name);
      
      order_anyway = 0;
    }
    
    if(order_anyway){
      /* Send order */
      engine_v2_order_alloc(order, uid, BUY, amount, CASH);
      engine_v2_set_order(engine, order);
      /* Display some info */
      metrics_n3_display(m, track_n3->track->name);
    }
  }
  
  /* Update ctx */
  ctx->last_month = month;
}

/* For each indicator */
static void feed_indicator_n3(struct engine_v2 *engine,
                              struct track_n3 *track_n3,
                              struct indicator_n3 *indicator_n3)
{
  struct metrics_n3 *m = (void*)indicator_n3;
  struct buy_monthly *ctx = track_n3->track->private; /* FIXME */
  
  switch(indicator_n3->indicator->uid){
  case METRICS_UID:
    ctx->metrics_ok = 1;
    if(pe != 0.0 && (m->pe > pe || m->pe < 0.0)) ctx->metrics_ok = 0;
    if(pbv != 0.0 && (m->pbv > pbv || m->pbv < 0.0)) ctx->metrics_ok = 0;
    if(de != 0.0 && (m->de > de || m->de < 0.0)) ctx->metrics_ok = 0;
    if(da != 0.0 && (m->da > da || m->da < 0.0)) ctx->metrics_ok = 0;
    break;

  case RS_UID:
    break; /* Nothing to do */
    
  default:
    PR_WARN("buy_monthly: unknown indicator %d\n",
            indicator_n3->indicator->uid);
  }
}

static struct engine_v2_interface engine_itf = {
  .feed_track_n3 = feed_track_n3,
  .feed_indicator_n3 = feed_indicator_n3,
};

static void custom_opt(struct timeline_v2 *t, char *opt, char *optarg)
{
  if(!strcmp(opt, "--modulo")) occurrence = atoi(optarg);
  if(!strcmp(opt, "--metrics")) metrics = 1;
  if(!strcmp(opt, "--pe")) sscanf(optarg, "%lf", &pe);
  if(!strcmp(opt, "--pbv")) sscanf(optarg, "%lf", &pbv);
  if(!strcmp(opt, "--de")) sscanf(optarg, "%lf", &de);
  if(!strcmp(opt, "--da")) sscanf(optarg, "%lf", &da);
  if(!strcmp(opt, "--ref")){ /* Or something */
    struct quotes *quotes;
    if(quotes_alloc(quotes, optarg, NULL)){
      struct track *track;
      track_alloc(track, -1, "ref", quotes);
      timeline_v2_add_track(t, track); /* Hope everything goes well */
    }
  }
}

static void customize_track(struct timeline_v2 *t, struct track *track)
{
  /* Allocate object */
  struct rs_roc *rs_roc;
  struct metrics *metrics;
  struct buy_monthly *buy_monthly;
  
  /* Add indicator(s) */
  if(metrics_alloc(metrics, METRICS_UID))
    track_add_indicator(track, &metrics->indicator);
  if(rs_roc_alloc(rs_roc, RS_UID, 20, -1))
    track_add_indicator(track, &rs_roc->indicator);
  
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
  engine_v2_init_ex(&engine, &timeline, argc, argv, &engine_itf);

  /* Start engine */
  engine_v2_run(&engine);

  /* Display info & pending orders */
  engine_v2_display_stats(&engine);
  engine_v2_display_pending_orders(&engine);
  
  /* Release engine & more */
  engine_v2_release(&engine);
  timeline_v2_release(&timeline);
  
  return 0;

 __catch__(usage):
  fprintf(stderr, "Usage: %s %s %s [--modulo m] [--check-income n]\n",
          argv[0], timeline_v2_ex_args, engine_v2_init_ex_args);
  return -1;
}
