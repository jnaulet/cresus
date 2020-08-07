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

#include "indicator/lowest.h"

static int occurrence = 1;

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

/* UIDs */
#define UID_LOWEST 1

/* For each indicator */
static void feed_indicator_n3(struct engine_v2 *engine,
                              struct track_n3 *track_n3,
                              struct indicator_n3 *indicator_n3)
{
  struct lowest_n3 *lowest_n3 = (void*)indicator_n3;
  unique_id_t uid = indicator_n3_indicator_uid(indicator_n3);
  
  switch(uid){
  case UID_LOWEST:
    PR_DBG("%s (lowest %.2lf)\n",
	   track_n3_str(track_n3),
	   lowest_n3->value);
    break;
    
  default:
    PR_ERR("feed_ndicator_n3: unknown indicator uid %d\n", uid);
    break;
  }
}

/* For each track */
static void feed_track_n3(struct engine_v2 *engine,
                          struct slice *slice,
                          struct track_n3 *track_n3)
{
  int month = TIME64_GET_MONTH(slice->time);
  unique_id_t uid = track_n3->track->uid;
  struct buy_monthly *ctx = track_n3->track->private;

  if(month != ctx->last_month && !(month % occurrence)){
    //PR_WARN("%s - BUY %d\n", track_n3_str(track_n3), amount);
    struct engine_v2_order *order;
    double amount = track_get_amount(track_n3->track, 500.0);
    engine_v2_order_alloc(order, uid, BUY, amount, CASH);
    engine_v2_set_order(engine, order);
  }

  /* Update ctx */
  ctx->last_month = month;
}

static struct engine_v2_interface engine_itf = {
  .feed_track_n3 = feed_track_n3,
  .feed_indicator_n3 = feed_indicator_n3
};

static void custom_opt(struct timeline_v2 *t, char *opt, char *optarg)
{
  if(!strcmp(opt, "--modulo")) occurrence = atoi(optarg);
}

static void customize_track(struct timeline_v2 *t, struct track *track)
{
  struct lowest *lowest;
  struct buy_monthly *buy_monthly;
  
  /* Add indicators per track */
  if(lowest_alloc(lowest, UID_LOWEST, 50))
    track_add_indicator(track, &lowest->indicator);

  /* Allocate object */
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
  fprintf(stderr, "Usage: %s %s %s [--module m]\n",
	  argv[0], timeline_v2_ex_args, engine_v2_init_ex_args);
  return -1;
}
