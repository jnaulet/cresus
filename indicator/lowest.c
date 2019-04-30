/*
 * Cresus EVO - lowest.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08.05.2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */
#include "lowest.h"

static void _lowest_reset_(struct indicator *i)
{
  struct lowest *ctx = (void*)i;
  /* Nothing to do */
}

void lowest_reset(struct lowest *ctx)
{
  _lowest_reset_(__indicator__(ctx));
}

static int lowest_feed(struct indicator *i,
		       struct timeline_track_n3 *e)
{
  int n = 0;
  struct lowest_n3 *n3;
  struct lowest *ctx = (void*)i;
  struct timeline_track_n3 *prev = NULL;
  
  if(lowest_n3_alloc(n3, i)){
    /* Init n3 value */
    n3->value = e->low;
    /* Find a lower value to exit */
    __list_for_each_prev__(e, prev){
      n3->value = MIN(prev->low, n3->value);
      /* End of loop */
      if(++n >= ctx->period)
	break;
    }
    
    /* Attach new n3 */
    timeline_track_n3_add_indicator_n3(e, __indicator_n3__(n3));
    return 1;
  }
  
 out:
  return 0;
}

int lowest_init(struct lowest *ctx, unique_id_t uid, int period)
{
  /* Super() */
  __indicator_init__(ctx, uid, lowest_feed, _lowest_reset_);
  __indicator_set_string__(ctx, "lowest");
  /* Init internals */
  ctx->period = period;
  return 0;
}

void lowest_release(struct lowest *ctx)
{
  __indicator_release__(ctx);
}
