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

static int smi_feed(struct indicator *i, struct candle *c) {
  
  double hi = 0.0;
  double lo = DBL_MAX;
  struct smi *s = __indicator_self__(i);

  memcpy(&s->pool[s->index], c, sizeof *c);
  s->index = (s->index + 1) % s->period;

  if(++s->count >= s->period){
    /* We got enough data. We get hi & lo */
    for(int i = s->period; i--;){
      hi = (s->pool[i].high > hi ? s->pool[i].high : hi);
      lo = (s->pool[i].low < lo ? s->pool[i].low : lo);
    }

    /* Midpoint Delta */
    double mpd = c->close - ((hi - lo) / 2.0);
    /* Double-smoothed midpoint delta */
    average_update(&s->smpd, average_update(&s->_smpd, mpd));

    /* Smoothed trading range */
    double str = average_update(&s->str, average_update(&s->_str,
							(hi - lo))) / 2.0;
    /* At last, Stochastic Momentum Index */
    s->value = (mpd / str) * 100.0;
  }

  /* TODO : events */

  return 0;
}

int smi_init(struct smi *s, int period, int smooth) {
  
  /* Super() */
  __indicator_super__(s, smi_feed);
  __indicator_set_string__(s, "smi[%d, %d]", period, smooth);
    
  s->count = 0;
  s->index = 0;
  s->period = period;

  if(!(s->pool = malloc(sizeof(*s->pool) * period)))
    return -1;
  
  average_init(&s->smpd, AVERAGE_EXP, period);
  average_init(&s->_smpd, AVERAGE_EXP, smooth);
  average_init(&s->str, AVERAGE_EXP, period);
  average_init(&s->_str, AVERAGE_EXP, smooth);
  
  /* memcpy(&s->pool[s->index++], seed, sizeof *seed); */
  return 0;
}

void smi_free(struct smi *s)
{
  __indicator_free__(s);
  average_free(&s->smpd);
  average_free(&s->_smpd);
  average_free(&s->str);
  average_free(&s->_str);
  
  if(s->pool)
    free(s->pool);
}

double smi_value(struct smi *s) {

  return s->value;
}
