//
//  stoploss.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 17/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include "stoploss.h"

int stoploss_init(struct stoploss *s, double percent) {
  
  /* Init parent */
  indicator_init(&s->parent, CANDLE_CLOSE, stoploss_feed);
  
  s->value = 0.0;
  s->trigger = 0;
  s->percent = percent;
  
  return 0;
}

void stoploss_free(struct stoploss *s) {
  
  indicator_free(&s->parent);
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

int stoploss_feed(struct indicator *i, const struct candle *candle) {
  
  struct stoploss *s = (struct stoploss*)i;
  
  if(s->type == STOPLOSS_UP){
    if(candle_get_value(candle, i->value) >= s->value){
      if(s->trigger)
        goto out;
    }
    
  }else{
    if(candle_get_value(candle, i->value) <= s->value){
      if(s->trigger)
        goto out;
    }
  }
  
  return 0;
  
out:
  /* indicator_throw_event(i, EVENT_STOPLOSS_HIT, candle); */
  stoploss_clear(s);
  return 1;
}
