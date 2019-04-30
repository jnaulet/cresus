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
			   struct timeline_track_n3 *e)
{  
  struct linear_reg_n3 *n3;
  struct linear_reg *ctx = (void*)i;
  struct timeline_track_n3 *prev;
  
  int n = ctx->period;
  int x = ctx->period;
  /* Some required variables */
  double a, b, value;
  double xysum = 0, xxsum = 0;
  double xsum = x, ysum = e->close;
  
  __list_for_each_prev__(e, prev){
    
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
    timeline_track_n3_add_indicator_n3(e, __indicator_n3__(n3));
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
  __indicator_init__(ctx, uid, linear_reg_feed, linear_reg_reset);
  __indicator_set_string__(ctx, "linear_reg[%d]", period);

  ctx->period = period;
  return 0;
}

void linear_reg_release(struct linear_reg *ctx)
{
  __indicator_release__(ctx);
}
