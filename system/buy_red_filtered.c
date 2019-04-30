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
#include <getopt.h>
#include <math.h>

#include "input/inwrap.h"
#include "engine/engine.h"
#include "engine/common_opt.h"
#include "framework/verbose.h"

typedef enum {
  STATE_NORMAL,
  STATE_PRIME
} state_t;

/* n consecutive red candles */
static int level_min = 1;
/* amount to buy */
static double amount = 500.0;
/* State machine */
static state_t state = STATE_NORMAL;

static int feed(struct engine *e,
		struct timeline *t,
		struct timeline_n3 *n3)
{
  /* Step by step loop */
  static int level = 0;
  struct candle *c = (void*)n3;
  
  /* Execute */
  if(candle_is_red(c)) level++;
  else level = 0;
  
  if(level >= level_min)
    state = STATE_PRIME;
  
  if(state == STATE_PRIME && !level){
    /* Trigger buy order */
    engine_set_order(e, BUY, amount, CASH, NULL);
    state = STATE_NORMAL;
  }
  
  return 0;
}

static struct timeline *timeline_create(const char *filename,
                                        const char *type)
{
  /*
   * Data
   */
  struct inwrap *inwrap;
  struct timeline *timeline;
  inwrap_t t = inwrap_t_from_str(type);
  
  if(inwrap_alloc(inwrap, filename, t)){
    if(timeline_alloc(timeline, "buy_red_filtered")){
      /* Ok */
      timeline_load(timeline, __input__(inwrap));
      return timeline;
    }
  }
  
  return NULL;
}

static void print_usage(const char *argv0)
{
  fprintf(stderr, "Usage: %s -o type filename\n", argv0);
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

  /* Check arguments */
  if(argc < 2)
    goto usage;

  /* Options */
  common_opt_init(&opt, "l:");
  while((c = common_opt_getopt(&opt, argc, argv)) != -1){
    switch(c){
    case 'l': level_min = atoi(optarg); break;
    default: break; /* Ignore */
    }
  }
  
  /* Command line params */
  filename = argv[optind];
  if(opt.fixed_amount.set) amount = opt.fixed_amount.i;
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
    __list_for_each__(&engine.list_position, p)
      if(p->status == POSITION_REQUESTED)
	PR_ERR("Buy now ! Quick ! Schnell !\n");
    
    /* TODO : Don't forget to release everything */
    engine_release(&engine);
  }
  
  return 0;

 usage:
  print_usage(argv[0]);
  return -1;
}
