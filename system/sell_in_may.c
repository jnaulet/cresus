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

#include "input/inwrap.h"
#include "engine/engine.h"
#include "framework/verbose.h"

static int amount = 250;
static int current_month = -1;
time64_t year_min = TIME64_INIT(1900, 1, 1, 0, 0, 0, 0);

/* Stats */
static int quiet = 0;

static int feed(struct engine *e,
		struct timeline *t,
		struct timeline_n3 *n3)
{
  /* Step by step loop */
  time64_t time = VAL_YEAR(year_min);
  struct candle *c = (void*)n3;

#define MONTH 6 /* June seems to work better */
  
  /* Execute */
  int month = TIME64_GET_MONTH(n3->time);
  if(month != current_month){
    if(month != MONTH) engine_set_order(e, BUY, amount, CASH, NULL);
    else engine_set_order(e, SELL, CASH, 100000, NULL); /* FIXME */
  }
  
  current_month = month;
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
    if(timeline_alloc(timeline, "sell_in_may")){
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
  char *filename, *type;
  
  struct timeline *t;
  struct engine engine;

  if(argc < 2){
    fprintf(stdout, "Usage: %s -o type filename\n", argv[0]);
    return -1;
  }

  while((c = getopt(argc, argv, "o:n:F:q")) != -1){
    switch(c){
    case 'o': type = optarg; break;
    case 'n': year_min = VAL_YEAR(atoi(optarg)); break;
    case 'F': amount = atoi(optarg); break;
    case 'q': quiet = 1; break;
    default:
      PR_ERR("Unknown option %c\n", c);
      return -1;
    }
  }

  /* Filename is only real param */
  filename = argv[optind];
  
  if((t = timeline_create(filename, type))){
    engine_init(&engine, t);
    /* Options */
    engine_set_transaction_fee(&engine, 2.50);
    engine_set_quiet(&engine, quiet);
    engine_set_start_time(&engine, year_min);

    /* Run */
    engine_run(&engine, feed);
    
    /* Print some info */
    engine_display_stats(&engine);
    
    /* TODO : Don't forget to release everything */
    engine_release(&engine);
  }
  
  return 0;
}
