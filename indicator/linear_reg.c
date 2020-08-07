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
			   struct track_n3 *e)
{
  struct list *l;
  struct linear_reg_n3 *n3;
  struct quotes_n3 *quotes = e->quotes;
  struct linear_reg *ctx = (void*)i;
  
  int n = ctx->period;
  int x = ctx->period;
  /* Some required variables */
  double a, b, value;
  double xysum = 0, xxsum = 0;
  double xsum = x, ysum = quotes->close;
  
  list_for_each_prev(&quotes->list, l){
    struct quotes_n3 *prev = (void*)l;
    double y = prev->close;
    
    if(!--x)
      break;
    
    /* Computation here */
    xsum = xsum + x;
    ysum = ysum + y;
    xysum = xysum + x * y;
    xxsum = xxsum + x * x;
  }
  
  /* Prepare new n3 */
  a = (n * xysum - xsum * ysum) / (n * xxsum - xsum * xsum);
  b = (ysum - a * xsum) / n;
  value = a * n + b; /* ax + b */
  
  /* Create new n3 */
  if(linear_reg_n3_alloc(n3, i, value)){
    track_n3_add_indicator_n3(e, &n3->indicator_n3);
    n3->a = a; /* For debug */
    n3->b = b; /* For debug */
    return 1;
  }
  
  return 0;
}

static void linear_reg_reset(struct indicator *i)
{
  /* Empty */
}

int linear_reg_init(struct linear_reg *ctx, unique_id_t uid, int period)
{  
  /* Super */
  indicator_init(&ctx->indicator, uid, linear_reg_feed, linear_reg_reset);
  indicator_set_string(&ctx->indicator, "linear_reg[%d]", period);
  ctx->period = period;
  return 0;
}

void linear_reg_release(struct linear_reg *ctx)
{
  indicator_release(&ctx->indicator);
}
