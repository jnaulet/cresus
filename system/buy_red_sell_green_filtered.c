/*
 * Cresus EVO - buy_red_sell_green.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 18/04/2018
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
  STATE_PRIME_BUY,
  STATE_PRIME_SELL
} state_t;

/* n consecutive candles */
static int level_buy_min = 1;
static int level_sell_min = 1;

#define candle_is_red(x) ((x)->close < (x)->open)
#define candle_is_green(x) ((x)->close > (x)->open)

struct buy_red_sell_green {
  state_t state;
  int level_buy;
  int level_sell;
};

int buy_red_sell_green_init(struct buy_red_sell_green *ctx)
{
  ctx->state = STATE_NORMAL;
  ctx->level_buy = 0;
  ctx->level_sell = 0;
  return 0;
}

#define buy_red_sell_green_alloc(ctx)					\
  DEFINE_ALLOC(struct buy_red_sell_green, ctx, buy_red_sell_green_init)

static void feed_track_n3(struct engine_v2 *engine,
                          struct slice *slice,
                          struct track_n3 *track_n3)
{
  struct engine_v2_order *order;
  struct quotes_n3 *p = track_n3->quotes;
  unique_id_t uid = track_n3->track->uid; /* FIXME */
  struct buy_red_sell_green *ctx = track_n3->track->private;
  double amount = track_get_amount(track_n3->track, 500.0);
  
  /* Execute */
  if(candle_is_red(p)){
    ctx->level_buy++;
    ctx->level_sell = 0;
  }else{
    ctx->level_sell++;
    ctx->level_buy = 0;
  }

  /* State machine */
  if(ctx->level_buy >= level_buy_min) ctx->state = STATE_PRIME_BUY;
  if(ctx->level_sell >= level_sell_min) ctx->state = STATE_PRIME_SELL;
  
  /* Trigger buy order */
  if(ctx->state == STATE_PRIME_BUY && !ctx->level_buy){
    engine_v2_order_alloc(order, uid, BUY, amount, CASH);
    engine_v2_set_order(engine, order);
    ctx->state = STATE_NORMAL;
  }
  
  /* Trigger sell order */
  if(ctx->state == STATE_PRIME_SELL && !ctx->level_sell){
    engine_v2_order_alloc(order, uid, SELL, amount, CASH);
    engine_v2_set_order(engine, order);
    ctx->state = STATE_NORMAL;
  }
}

static struct engine_v2_interface engine_itf = {
  .feed_track_n3 = feed_track_n3
};

static void custom_opt(struct timeline_v2 *t, char *opt, char *optarg)
{
  if(!strcmp(opt, "--buy")) level_buy_min = atoi(optarg);
  if(!strcmp(opt, "--sell")) level_sell_min = atoi(optarg);
}

static void customize_track(struct timeline_v2 *t, struct track *track)
{
  struct buy_red_sell_green *buy_red_sell_green;
  track->private = buy_red_sell_green_alloc(buy_red_sell_green);
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

  timeline_v2_init_ex(&timeline, argc, argv, &timeline_itf);
  engine_v2_init_ex(&engine, &timeline, argc, argv);
  
  /* Run */
  engine_v2_run(&engine, &engine_itf);
  
  /* Release engine & more */
  engine_v2_release(&engine);
  timeline_v2_release(&timeline);
  
  return 0;
  
 __catch__(usage):
  fprintf(stdout, "Usage: %s %s %s [--buy buy_min] "	\
	  "[--sell sell_min]\n",
	  argv[0], timeline_v2_ex_args, engine_v2_init_ex_args);
  return -1;
}
