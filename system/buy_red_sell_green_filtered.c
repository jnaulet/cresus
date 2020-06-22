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
#include <getopt.h>
#include <math.h>
#include <libgen.h>

#include "engine/engine_v2.h"
#include "engine/common_opt.h"
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
/* amount to buy */
static double amount = 500.0;
/* State machine */
static state_t state = STATE_NORMAL;

#define candle_is_red(x) ((x)->close < (x)->open)
#define candle_is_green(x) ((x)->close > (x)->open)

static void feed_track_n3(struct engine_v2 *engine,
                          struct slice *slice,
                          struct track_n3 *track_n3)
{
  struct engine_v2_order *order;
  struct price_n3 *p = track_n3->price;
  unique_id_t uid = track_n3->track->uid; /* FIXME */
  /* Step by step loop */
  static int level_buy = 0, level_sell = 0;
  
  /* Execute */
  if(candle_is_red(p)){
    level_buy++;
    level_sell = 0;
  }else{
    level_sell++;
    level_buy = 0;
  }

  /* State machine */
  if(level_buy >= level_buy_min) state = STATE_PRIME_BUY;
  if(level_sell >= level_sell_min) state = STATE_PRIME_SELL;
  
  /* Trigger buy order */
  if(state == STATE_PRIME_BUY && !level_buy){
    engine_v2_order_alloc(order, uid, BUY, amount, CASH);
    engine_v2_set_order(engine, order);
    state = STATE_NORMAL;
  }
  
  /* Trigger sell order */
  if(state == STATE_PRIME_SELL && !level_sell){
    engine_v2_order_alloc(order, uid, SELL, amount, CASH);
    engine_v2_set_order(engine, order);
    state = STATE_NORMAL;
  }
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
    __try__(!track_alloc(track, track_uid, basename(filename), price, NULL), err);
    return timeline_add_track(t, track);
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

  timeline_init(&timeline);
  common_opt_init(&opt, "b:s:F:");
  
  while((c = common_opt_getopt_linear(&opt, argc, argv, &optarg)) != -1){
    switch(c){
    case 'b': level_buy_min = atoi(optarg); break;
    case 's': level_sell_min = atoi(optarg); break;
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
  fprintf(stdout, "Usage: %s -o type filename\n", argv[0]);
  return -1;
}
