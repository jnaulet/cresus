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
#include "engine/common_opt.h"
#include "framework/verbose.h"

double last_close;

static int feed(struct engine *e,
		struct timeline *t,
		struct timeline_n3 *n3)
{
  /* Step by step loop */
  static int n = 0;
  time64_t time = VAL_YEAR(year_min);
  struct candle *c = __timeline_n3_self__(n3);
  
  if(TIME64CMP(n3->time, time, GR_YEAR) < 0)
    goto out;
  
  /* Execute */
  if(candle_is_red(c))
    engine_set_order(e, BUY, 500, CASH, NULL);

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
  
  if(inwrap_alloc(inwrap, filename, t, TIME64_MIN, TIME64_MAX)){
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
  char *filename;
  
  struct timeline *t;
  struct engine engine;

  if(argc < 2)
    goto usage;

  
  common_opt_init(&opt, "");
  while((c = common_opt_getopt(&opt, argc, argv)) != -1){} /* ! */

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

 usage:
  fprintf(stdout, "Usage: %s -o type filename\n", argv[0]);
  return -1;
}
