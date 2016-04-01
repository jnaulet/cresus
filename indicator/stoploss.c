/*
 * Cresus EVO - stoploss.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/17/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "stoploss.h"

static int stoploss_feed(struct indicator *i, struct candle *candle) {
  
  struct stoploss *s = __indicator_self__(i);
  
  if(s->type == STOPLOSS_UP){
    if(candle->close >= s->value){
      if(s->trigger)
        goto out;
    }
    
  }else{
    if(candle->close <= s->value){
      if(s->trigger)
        goto out;
    }
  }
  
  return 0;
  
out:
  indicator_set_event(i, candle, STOPLOSS_EVENT_HIT);
  stoploss_clear(s);
  return 1;
}

int stoploss_init(struct stoploss *s, double percent) {
  
  /* Init parent */
  __indicator_super__(s, stoploss_feed);
  __indicator_set_string__(s, "stoploss[%.2f]", percent);
  
  s->value = 0.0;
  s->trigger = 0;
  s->percent = percent;
  
  return 0;
}

void stoploss_free(struct stoploss *s) {
  
  __indicator_free__(s);
  s->value = 0.0;
  s->trigger = 0;
}

void stoploss_set(struct stoploss *s, stoploss_t type, double value) {
  
  if(type == STOPLOSS_UP)
    s->value = value * (1.0 + (s->percent / 100.0));
  else
    s->value = value * (1.0 - (s->percent / 100.0));
  
  /* Trigger signal */
  s->trigger = 1;
  s->type = type;
}

void stoploss_clear(struct stoploss *s) {
  
  s->value = 0.0;
  s->trigger = 0;
}
