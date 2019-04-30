/*
 * Cresus EVO - smi.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>

#include "smi.h"

static int smi_feed(struct indicator *i, struct timeline_track_n3 *e)
{  
  double hi = 0.0;
  double lo = DBL_MAX;
  struct smi *ctx = (void*)i;
  struct candle *c = (void*)e;

  memcpy(&ctx->pool[ctx->index], c, sizeof *c);
  ctx->index = (ctx->index + 1) % ctx->period;

  if(++ctx->count >= ctx->period){
    /* We got enough data. We get hi & lo */
    for(int i = ctx->period; i--;){
      hi = (ctx->pool[i].high > hi ? ctx->pool[i].high : hi);
      lo = (ctx->pool[i].low < lo ? ctx->pool[i].low : lo);
    }

    /* Midpoint Delta */
    double mpd = c->close - ((hi - lo) / 2.0);
    /* Double-smoothed midpoint delta */
    average_update(&ctx->smpd, average_update(&ctx->_smpd, mpd));

    /* Smoothed trading range */
    double str = average_update(&ctx->str, average_update(&ctx->_str,
                                                          (hi - lo))) / 2.0;
    /* At last, Stochastic Momentum Index */
    ctx->value = (mpd / str) * 100.0;
  }

  /* TODO : events */

  return 0;
}

static void smi_reset(struct indicator *i) {

  struct smi *ctx = (void*)i;
  /* Reset internal average */
  ctx->count = 0;
  ctx->index = 0;
  /* Reset other averages */
  average_reset(&ctx->smpd);
  average_reset(&ctx->_smpd);
  average_reset(&ctx->str);
  average_reset(&ctx->_str);
}

int smi_init(struct smi *ctx, unique_id_t id, int period, int smooth) {
  
  /* Super() */
  __indicator_init__(ctx, id, smi_feed, smi_reset);
  __indicator_set_string__(ctx, "smi[%d, %d]", period, smooth);
    
  ctx->count = 0;
  ctx->index = 0;
  ctx->period = period;

  if(!(ctx->pool = malloc(sizeof(*ctx->pool) * period)))
    return -1;
  
  average_init(&ctx->smpd, AVERAGE_EXP, period);
  average_init(&ctx->_smpd, AVERAGE_EXP, smooth);
  average_init(&ctx->str, AVERAGE_EXP, period);
  average_init(&ctx->_str, AVERAGE_EXP, smooth);
  
  /* memcpy(&ctx->pool[ctx->index++], seed, sizeof *ctxeed); */
  return 0;
}

void smi_release(struct smi *ctx)
{
  __indicator_release__(ctx);
  average_release(&ctx->smpd);
  average_release(&ctx->_smpd);
  average_release(&ctx->str);
  average_release(&ctx->_str);
  
  if(ctx->pool)
    free(ctx->pool);
}

double smi_value(struct smi *ctx) {

  return ctx->value;
}
