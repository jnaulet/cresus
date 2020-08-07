/*
 * Cresus EVO - buy_red_filtered.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 07/04/2016
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
#include "framework/timeline_v2.h"

typedef enum {
  STATE_NORMAL,
  STATE_PRIME
} state_t;

/* n consecutive red candles */
static int level_min = 1;
#define candle_is_red(x) ((x)->close < (x)->open)

struct buy_red_filtered {
  int level;
  state_t state;
};

static int buy_red_filtered_init(struct buy_red_filtered *ctx)
{
  ctx->level = 0;
  ctx->state = STATE_NORMAL;
  return 0;
}

#define buy_red_filtered_alloc(ctx)					\
  DEFINE_ALLOC(struct buy_red_filtered, ctx, buy_red_filtered_init)

static void feed_track_n3(struct engine_v2 *engine,
                          struct slice *slice,
                          struct track_n3 *track_n3)
{
  /* Step by step loop */
  struct quotes_n3 *p = track_n3->quotes;
  unique_id_t uid = track_n3->track->uid; /* FIXME */
  struct buy_red_filtered *ctx = track_n3->track->private;
  
  /* Execute */
  if(candle_is_red(p)) ctx->level++;
  else ctx->level = 0;
  
  if(ctx->level >= level_min)
    ctx->state = STATE_PRIME;
  
  if(ctx->state == STATE_PRIME && !ctx->level){
    /* Trigger buy order */
    struct engine_v2_order *order;
    double amount = track_get_amount(track_n3->track, 500.0);
    engine_v2_order_alloc(order, uid, BUY, amount, CASH);
    engine_v2_set_order(engine, order);
    /* Back to normal */
    ctx->state = STATE_NORMAL;
  }
}

static struct engine_v2_interface engine_itf = {
  .feed_track_n3 = feed_track_n3
};

static void custom_opt(struct timeline_v2 *t, char *opt, char *optarg)
{
  if(!strcmp(opt, "--level")) level_min = atoi(optarg);
}

static void customize_track(struct timeline_v2 *t, struct track *track)
{
  struct buy_red_filtered *buy_red_filtered;
  track->private = buy_red_filtered_alloc(buy_red_filtered);
}

static struct timeline_v2_ex_interface timeline_itf = {
   .custom_opt = custom_opt,
   .customize_track = customize_track
};

static void print_usage(const char *argv0)
{
  fprintf(stderr, "Usage: %s %s %s\n", argv0,
	  timeline_v2_ex_args, engine_v2_init_ex_args);
}

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
  
  /* Release engine & more */
  engine_v2_release(&engine);
  timeline_v2_release(&timeline);
  
  return 0;

 __catch__(usage):
  print_usage(argv[0]);
  return -1;
}
