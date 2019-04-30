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

#include "input/inwrap.h"
#include "engine/engine.h"
#include "engine/common_opt.h"
#include "framework/verbose.h"

typedef enum {
  STATE_NORMAL,
  STATE_PRIME_BUY,
  STATE_PRIME_SELL
} state_t;

static int level_buy_min = 1;
static int level_sell_min = 1;
static state_t state = STATE_NORMAL;

static int feed(struct engine *e,
		struct timeline *t,
		struct timeline_n3 *n3)
{
  /* Step by step loop */
  static int level_buy = 0, level_sell = 0;
  struct candle *c = (void*)n3;
  
  /* Execute */
  if(candle_is_red(c)){
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
    engine_set_order(e, BUY, 500, CASH, NULL);
    state = STATE_NORMAL;
  }
  
  /* Trigger sell order */
  if(state == STATE_PRIME_SELL && !level_sell){
    engine_set_order(e, SELL, 500, CASH, NULL);
    state = STATE_NORMAL;
  }
  
  return 0;
}

static struct timeline *timeline_create(const char *filename, const char *type)
{
  /*
   * Data
   */
  struct inwrap *inwrap;
  struct timeline *timeline;
  inwrap_t t = inwrap_t_from_str(type);
  
  if(inwrap_alloc(inwrap, filename, t)){
    if(timeline_alloc(timeline, "buy_red_sell_green_filtered")){
      /* Ok */
      timeline_load(timeline, __input__(inwrap));
      return timeline;
    }
  }
  
  return NULL;
}

int main(int argc, char **argv)
{
  VERBOSE_LEVEL(INFO);
  
  /*
   * Data
   */
  int c;
  char *filename;
  struct common_opt opt;
  
  struct timeline *t;
  struct engine engine;

  if(argc < 2)
    goto usage;

  /* Options */
  common_opt_init(&opt, "b:s:");
  while((c = common_opt_getopt(&opt, argc, argv)) != -1){
    switch(c){
    case 'b': level_buy_min = atoi(optarg); break;
    case 's': level_sell_min = atoi(optarg); break;
    default: break;
    }
  }

  /* Command line params */
  filename = argv[optind];
  if(!opt.input_type.set) goto usage;
  
  if((t = timeline_create(filename, opt.input_type.s))){
    /* Engine setup */
    engine_init(&engine, t);
    engine_set_common_opt(&engine, &opt);
    /* Run */
    engine_run(&engine, feed);

    /* Print some info */
    engine_display_stats(&engine);

    /* Are there pending orders ? (FIXME : dedicated function in engine ?) */
    struct position *p;
    __list_for_each__(&engine.list_position, p){
      if(p->status == POSITION_REQUESTED){
	switch(p->type){
	case BUY: PR_ERR("Buy now ! Quick ! Schnell !\n");
	case SELL: PR_ERR("Sell now ! Quick ! Schnell !\n");
	default: PR_ERR("C'mon do something\n");
	}
      }
    }
    
    /* TODO : Don't forget to release everything */
    engine_release(&engine);
  }
  
  return 0;

 usage:
  fprintf(stdout, "Usage: %s -o type filename\n", argv[0]);
  return -1;
}
