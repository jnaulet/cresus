/*
 * Cresus EVO - macd.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>

#include "macd.h"

static int macd_feed(struct indicator *i, struct timeline_track_n3 *e)
{  
  struct macd *ctx = (void*)i;

  /* Set 2 main averages */
  double fast = average_update(&ctx->fast, e->close);
  double slow = average_update(&ctx->slow, e->close);
  if(average_is_available(&ctx->fast) &&
     average_is_available(&ctx->slow)){
    /* Compute signal */
    double value = (fast - slow);
    double signal = average_update(&ctx->signal, value);
    if(average_is_available(&ctx->signal)){
      struct macd_n3 *n3;
      if(macd_n3_alloc(n3, i, value, signal))
	timeline_track_n3_add_indicator_n3(e, __indicator_n3__(n3));
    }
  }
  
  /* State machine & events here */
  return 0;
}

static void macd_reset(struct indicator *i)
{
  struct macd *ctx = (void*)i;
  /* Reset averages */
  average_reset(&ctx->fast);
  average_reset(&ctx->slow);
  average_reset(&ctx->signal);
}

int macd_init(struct macd *ctx, unique_id_t uid,
	      int fast_p, int slow_p, int signal_p)
{
  /* Super */
  __indicator_init__(ctx, uid, macd_feed, macd_reset);
  __indicator_set_string__(ctx, "macd[%d,%d,%d]", fast_p, slow_p, signal_p);
  
  /* Seed */
  average_init(&ctx->fast, AVERAGE_EXP, fast_p);
  average_init(&ctx->slow, AVERAGE_EXP, slow_p);
  average_init(&ctx->signal, AVERAGE_EXP, signal_p);
  
  return 0;
}

void macd_release(struct macd *ctx)
{
  __indicator_release__(ctx);
  average_release(&ctx->fast);
  average_release(&ctx->slow);
  average_release(&ctx->signal);
}
