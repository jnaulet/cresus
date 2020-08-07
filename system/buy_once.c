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

#include "engine/engine_v2.h"
#include "framework/verbose.h"
#include "framework/quotes.h"

struct buy_once {
  int once;
};

static int buy_once_init(struct buy_once *ctx)
{
  ctx->once = 0;
  return 0;
}

#define buy_once_alloc(ctx)				\
  DEFINE_ALLOC(struct buy_once, ctx, buy_once_init)

/* For each track */
static void feed_track_n3(struct engine_v2 *engine,
                          struct slice *slice,
                          struct track_n3 *track_n3)
{
  unique_id_t uid = track_n3->track->uid;
  struct buy_once *ctx = track_n3->track->private;
  double amount = track_get_amount(track_n3->track, 500.0);
  
  if(!ctx->once){
    struct engine_v2_order *order;
    engine_v2_order_alloc(order, uid, BUY, amount, CASH);
    engine_v2_set_order(engine, order);
    /* Once done */
    ctx->once = 1;
  }
}

static struct engine_v2_interface engine_itf = {
   .feed_track_n3 = feed_track_n3
};

static void customize_track(struct timeline_v2 *t, struct track *track)
{
  struct buy_once *buy_once;
  track->private = buy_once_alloc(buy_once);
}

static struct timeline_v2_ex_interface timeline_itf = {
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

  /* Options */
  timeline_v2_init_ex(&timeline, argc, argv, &timeline_itf);
  engine_v2_init_ex(&engine, &timeline, argc, argv);

  /* Run */
  engine_v2_run(&engine, &engine_itf);
  
  /* Release engine & more */
  engine_v2_release(&engine);
  timeline_v2_release(&timeline);
  
  return 0;

 __catch__(usage):
  fprintf(stderr, "Usage: %s %s %s\n", argv[0],
	  timeline_v2_ex_args, engine_v2_init_ex_args);
  return -1;
}
