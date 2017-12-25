/*
 * Cresus EVO - linear_reg.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 18/03/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "linear_reg.h"

static int linear_reg_feed(struct indicator *i,
			   struct timeline_entry *e)
{  
  struct linear_reg_entry *entry;
  struct timeline_entry *prev = NULL;
  struct linear_reg *l = __indicator_self__(i);
  struct candle *c = __timeline_entry_self__(e);

  int n = l->period;
  int x = l->period;
  /* Some required variables */
  double a, b, value;
  double xysum = 0, xxsum = 0;
  double xsum = x, ysum = c->close;
  
  __list_for_each_prev__(__list__(e), prev){
    struct candle *p = __timeline_entry_self__(prev);
    double y = p->close;
    
    if(!--x)
      break;

    /* Computation here */
    xsum = xsum + x;
    ysum = ysum + y;
    xysum = xysum + x * y;
    xxsum = xxsum + x * x;
  }

  /* Prepare new entry */
  a = (n * xysum - xsum * ysum) / (n * xxsum - xsum * xsum);
  b = (ysum - a * xsum) / n;
  value = a * n + b; /* ax + b */
  
  /* Create new entry */
  if(linear_reg_entry_alloc(entry, i, value)){
    candle_add_indicator_entry(c, __indicator_entry__(entry));
    entry->a = a; /* For debug */
    entry->b = b; /* For debug */
    return 1;
  }
  
  return 0;
}

static void linear_reg_reset(struct indicator *i)
{
  /* Empty */
}

int linear_reg_init(struct linear_reg *l, indicator_id_t id, int period)
{  
  /* Super */
  __indicator_super__(l, id, linear_reg_feed, linear_reg_reset);
  __indicator_set_string__(l, "%lr[%d]", period);

  l->period = period;
  return 0;
}

void linear_reg_release(struct linear_reg *l)
{
  __indicator_release__(l);
}
