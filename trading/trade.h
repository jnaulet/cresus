//
//  trade.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 22/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#ifndef __Cresus_EVO__trade__
#define __Cresus_EVO__trade__

#include "framework/candle.h"

typedef enum {
  TRADE_NONE,
  TRADE_LONG,
  TRADE_SHORT
} trade_t;

struct trade {
  trade_t type;
  candle_value_t value;
  
  double buy_price;
  double cur_price;
  int number;
};

int trade_init(struct trade *t, trade_t type,
               const struct candle *candle,
               candle_value_t value, int number);

void trade_free(struct trade *t);

void trade_feed(struct trade *t, const struct candle *candle);
double trade_get_value(struct trade *t);

#endif /* defined(__Cresus_EVO__trade__) */
