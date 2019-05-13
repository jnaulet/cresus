/*
 * Cresus EVO - buy_once.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 27/03/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 *
 * Development sample code
 * Used to determine if interfaces are coherent for the "user"
 * And maybe to implement some kind of script commands 
 *
 */

#include <stdio.h>
#include <libgen.h>

#include "engine/engine_v2.h"
#include "engine/common_opt.h"

#include "framework/verbose.h"
#include "framework/timeline.h"
#include "input/input_wrapper.h"

static int amount = 500;

struct buy_once {
  int once;
};

static int buy_once_init(struct buy_once *ctx)
{
  ctx->once = 0;
  return 0;
}

#define buy_once_alloc(ctx)					\
  DEFINE_ALLOC(struct buy_once, ctx, buy_once_init)

/* For each track */
static void feed_track_n3(struct engine_v2 *engine,
                          struct timeline_slice *slice,
                          struct timeline_track_n3 *track_n3)
{
  unique_id_t uid = __slist_uid_uid__(track_n3->track);
  struct buy_once *ctx = timeline_track_n3_track_private(track_n3);

  if(!ctx->once){
    struct engine_v2_order *order;
    engine_v2_order_alloc(order, uid, BUY, amount, CASH);
    engine_v2_set_order(engine, order);
    /* Once done */
    ctx->once = 1;
  }
}

static struct engine_v2_interface itf = {
  .feed_track_n3 = feed_track_n3
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
    struct buy_once *ctx;
    struct timeline_track *track;
    __try__(!buy_once_alloc(ctx), err);
    __try__(!timeline_track_alloc(track, track_uid,
                                  basename(filename), ctx), err);
    /* No indicators */
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
  while((c = common_opt_getopt_linear(&opt, argc, argv, &optarg)) != -1)
    if(c == '-') timeline_create(&timeline, optarg, n++);
  /* Fixed amount opt */
  if(opt.fixed_amount.set)
    amount = opt.fixed_amount.i;
  
  /* Execute timeline data */
  timeline_run_and_sync(&timeline);
  
  /* New engine */
  engine_v2_init(&engine, &timeline);
  engine_v2_set_common_opt(&engine, &opt);

  /* Run */
  engine_v2_run(&engine, &itf);
  
  /* Release engine & more */
  engine_v2_release(&engine);
  timeline_release(&timeline);
  
  return 0;

 __catch__(usage):
  fprintf(stderr, "Usage: %s [-F amount] [-c csv] file.ext\n", argv[0]);
  return -1;
}
