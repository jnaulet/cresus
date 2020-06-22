/*
 * Cresus EVO - sell_in_may.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 20/04/2018
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
#include "framework/price.h"

static int amount = 250;
static int month = 5; /* Defautl : sell in May. June seems better :/ */

struct sell_in_may {
  int cur_month;
};

static int sell_in_may_init(struct sell_in_may *ctx)
{
  ctx->cur_month = -1;
  return 0;
}

#define sell_in_may_alloc(ctx)                                  \
  DEFINE_ALLOC(struct sell_in_may, ctx, sell_in_may_init)

static void feed_track_n3(struct engine_v2 *engine,
                          struct slice *slice,
                          struct track_n3 *track_n3)
{
  struct engine_v2_order *order;
  int cur_month = TIME64_GET_MONTH(slice->time);
  unique_id_t uid = track_n3->track->uid;
  struct sell_in_may *ctx = track_n3->track->private;
  
  if(cur_month != ctx->cur_month){
    if(cur_month != month){
      engine_v2_order_alloc(order, uid, BUY, amount, CASH);
      engine_v2_set_order(engine, order);
    }else{
      /* FIXME: create a SELL_ALL order ? */
      engine_v2_order_alloc(order, uid, SELL, INT_MAX, CASH);
      engine_v2_set_order(engine, order);
    }
  }
  
  ctx->cur_month = cur_month;
}

static struct engine_v2_interface itf = {
  .feed_track_n3 = feed_track_n3
};

static int timeline_create(struct timeline *t,
                           char *filename,
                           unique_id_t track_uid)
{
  struct price *price;
  
  if((price = price_alloc(price, filename, NULL))){
    /* Create tracks */
    struct track *track;
    struct sell_in_may *ctx;
    __try__(!sell_in_may_alloc(ctx), err);
    __try__(!track_alloc(track, track_uid, basename(filename), price, ctx), err);
    
    /* Add to timeline */
    timeline_add_track(t, track);
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
    case 'm': month = atoi(optarg); break;
    case 'F': amount = atoi(optarg); break;
    case '-': timeline_create(&timeline, optarg, n++);
    }
  }
  
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
  fprintf(stdout, "Usage: %s -o type filename [-m month]\n", argv[0]);
  return -1;
}
