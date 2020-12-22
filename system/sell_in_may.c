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
#include <math.h>

#include "engine/engine_v2.h"
#include "framework/verbose.h"
#include "framework/quotes.h"

static int month = 5; /* Default : sell in May. June seems better :/ */

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
  unique_id_t uid = track_n3->track->uid;
  struct sell_in_may *ctx = track_n3->track->private;
  double amount = track_get_amount(track_n3->track, 500.0);

  struct tm *tm = localtime(&slice->time);
  int cur_month = tm->tm_mon + 1;
  
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

static struct engine_v2_interface engine_itf = {
  .feed_track_n3 = feed_track_n3
};

static void custom_opt(struct timeline_v2 *t, char *opt, char *optarg)
{
  if(!strcmp(opt, "--month")) month = atoi(optarg);
}


static void customize_track(struct timeline_v2 *t, struct track *track)
{
  struct sell_in_may *sell_in_may;
  track->private = sell_in_may_alloc(sell_in_may);
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

  /* Basics */
  timeline_v2_init_ex(&timeline, argc, argv, &timeline_itf);
  engine_v2_init_ex(&engine, &timeline, argc, argv);
  
  /* Run */
  engine_v2_run(&engine, &engine_itf);

  /* "normal" output */
  engine_v2_display_stats(&engine);
  engine_v2_display_pending_orders(&engine);
  
  /* Release engine & more */
  engine_v2_release(&engine);
  timeline_v2_release(&timeline);
  
  return 0;
  
 __catch__(usage):
  fprintf(stdout, "Usage: %s %s %s [--month m]\n", argv[0],
          timeline_v2_ex_args, engine_v2_init_ex_args);
  return -1;
}
