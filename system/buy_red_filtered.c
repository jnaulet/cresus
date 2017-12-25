/*
 * Cresus EVO - basics.c
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
#include "framework/verbose.h"

static double last_close;
static int year_min = 1900;
static int level_min = 1;

static int feed(struct engine *e,
		struct timeline *t,
		struct timeline_entry *entry)
{
  /* Step by step loop */
  static int n = 0, level = 0;
  time_info_t time = VAL_YEAR(year_min);
  struct candle *c = __timeline_entry_self__(entry);
  
  if(TIMECMP(entry->time, time, GRANULARITY_YEAR) < 0)
    goto out;
  
  /* Execute */
  if(candle_is_red(c)) level++;
  else level = 0;
  
  if(level >= level_min){
    PR_INFO("%s - BUY 500.0 CASH (%d)\n", candle_str(c), ++n);
    engine_place_order(e, ORDER_BUY, ORDER_BY_AMOUNT, 500);
  }
  
 out:
  last_close = c->close; /* ! */
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
  
  if(inwrap_alloc(inwrap, filename, t, TIME_MIN, TIME_MAX)){
    if(timeline_alloc(timeline, "buy_red_no_filter", __input__(inwrap))){
      /* Ok */
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

  while((c = getopt(argc, argv, "o:n:l:")) != -1){
    switch(c){
    case 'o': type = optarg; break;
    case 'n': year_min = atoi(optarg); break;
    case 'l': level_min = atoi(optarg); break;
    default:
      PR_ERR("Unknown option %c\n", c);
      return -1;
    }
  }

  /* Filename is only real param */
  filename = argv[optind];
  
  if((t = timeline_create(filename, type))){
    engine_init(&engine, t);
    engine_run(&engine, feed);
    /* print some info */
    double amount = fabs(engine.amount);
    double total = engine.npos * last_close;
    double pcent = ((total / amount) - 1.0) * 100.0;
    PR_ERR("Have %.2lf positions paid %.2lf worth %.2lf (%.2lf%%)\n",
	   engine.npos, amount, total, pcent);
    
    /* TODO : Don't forget to release everything */
    engine_release(&engine);
  }
  
  return 0;
}
