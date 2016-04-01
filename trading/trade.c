/*
 * Cresus EVO - trade.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

//
//  trade.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 22/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include "trade.h"

int trade_init(struct trade *t, trade_t type,
               const struct candle *candle,
               candle_value_t value, int number) {
  
  t->type = type;
  t->value = value;
  t->number = number;
  
  /* FIXME */
  if(candle != NULL)
    t->buy_price = candle_get_value(candle, value);
  
  return 0;
}

void trade_free(struct trade *t) {
  
  t->type = TRADE_NONE;
  t->number = 0;
}

void trade_feed(struct trade *t, const struct candle *candle){
  
  t->cur_price = candle_get_value(candle, t->value);
}

double trade_get_value(struct trade *t) {
  
  if(t->type == TRADE_LONG)
    return (t->cur_price - t->buy_price) * t->number;
  if(t->type == TRADE_SHORT)
    return (t->buy_price - t->cur_price) * t->number;
  
  return 0.0;
}