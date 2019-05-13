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
#include <getopt.h>
#include <math.h>
#include <libgen.h>

#include "engine/engine_v2.h"
#include "engine/common_opt.h"

#include "framework/verbose.h"
#include "framework/timeline.h"
#include "input/input_wrapper.h"

#include "indicator/lowest.h"

static int amount = 250;
static int occurrence = 1;

struct buy_monthly {
  int last_month;
};

static int buy_monthly_init(struct buy_monthly *ctx)
{
  ctx->last_month = -1;
  return 0;
}

#define buy_monthly_alloc(ctx)                                  \
  DEFINE_ALLOC(struct buy_monthly, ctx, buy_monthly_init)

/* UIDs */
#define UID_LOWEST 1

/* For each indicator */
static void feed_indicator_n3(struct engine_v2 *engine,
                              struct timeline_track_n3 *track_n3,
                              struct indicator_n3 *indicator_n3)
{
  struct lowest_n3 *lowest_n3 = (void*)indicator_n3;
  unique_id_t uid = indicator_n3_indicator_uid(indicator_n3);
  
  switch(uid){
  case UID_LOWEST:
    PR_DBG("%s (lowest %.2lf)\n",
	   timeline_track_n3_str(track_n3),
	   lowest_n3->value);
    break;
    
  default:
    PR_ERR("feed_ndicator_n3: unknown indicator uid %d\n", uid);
    break;
  }
}

/* For each track */
static void feed_track_n3(struct engine_v2 *engine,
                          struct timeline_slice *slice,
                          struct timeline_track_n3 *track_n3)
{
  int month = TIME64_GET_MONTH(slice->time);
  unique_id_t uid = __slist_uid_uid__(track_n3->track);
  struct buy_monthly *ctx = timeline_track_n3_track_private(track_n3);
  
  if(month != ctx->last_month && !(month % occurrence)){
    //PR_WARN("%s - BUY %d\n", timeline_track_n3_str(track_n3), amount);
    struct engine_v2_order *order;
    engine_v2_order_alloc(order, uid, BUY, 500.0, CASH);
    engine_v2_set_order(engine, order);
  }

  /* Update ctx */
  ctx->last_month = month;
}

static struct engine_v2_interface itf = {
  .feed_track_n3 = feed_track_n3,
  .feed_indicator_n3 = feed_indicator_n3
};

static int timeline_create(struct timeline *t, char *filename,
                           unique_id_t track_uid)
{
  /*
   * Data
   */
  struct input *input;
  if((input = input_wrapper_create_by_ext(filename))){
    /* Create tracks */
    struct buy_monthly *ctx;
    struct timeline_track *track;
    __try__(!buy_monthly_alloc(ctx), err);
    __try__(!timeline_track_alloc(track, track_uid,
                                  basename(filename), ctx), err);
    /* Create indicators */
    struct lowest *lowest;
    __try__(!lowest_alloc(lowest, UID_LOWEST, 50), err);
    timeline_track_add_indicator(track, __indicator__(lowest));
    /* Add to timeline */
    timeline_add_track(t, track, input);
    return 0;
  }
  
 __catch__(err):
  return -1;
}

int main(int argc, char **argv)
{
  VERBOSE_LEVEL(INFO);
  
  /*
   * Data
   */
  int c, n = 0;
  char *optarg;

  struct common_opt opt;
  struct engine_v2 engine;
  struct timeline timeline;

  /* Check arguments */
  __try__(argc < 2, usage);

  /* Options */
  timeline_init(&timeline);
  common_opt_init(&opt, "m:");
  
  while((c = common_opt_getopt_linear(&opt, argc, argv, &optarg)) != -1){
    switch(c){
    case 'm': occurrence = atoi(optarg); break;
    case 'F': amount = atoi(optarg); break;
    case '-': timeline_create(&timeline, optarg, n++); break;
    }
  }
  
  /* Execute timeline */
  timeline_run_and_sync(&timeline);
  /* Start engine */
  engine_v2_init(&engine, &timeline);
  engine_v2_set_common_opt(&engine, &opt);
  /* Run */
  engine_v2_run(&engine, &itf);
  
  /* Release engine & more */
  engine_v2_release(&engine);
  timeline_release(&timeline);
  
  return 0;

 __catch__(usage):
  fprintf(stderr, "Usage: %s -o type filename [-m occurrence]\n", argv[0]);
  return -1;
}
