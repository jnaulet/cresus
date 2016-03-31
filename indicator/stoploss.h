//
//  stoploss.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 17/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#ifndef STOPLOSS_H
#define STOPLOSS_H

#include "framework/indicator.h"
#include "framework/candle.h"

typedef enum {
  STOPLOSS_UP,
  STOPLOSS_DOWN
} stoploss_t;

struct stoploss {
  /* Inherits from indicator */
  struct indicator parent;
  
  /* Own data */
  stoploss_t type;
  double percent;
  double value;
  int trigger;
};

int stoploss_init(struct stoploss *s, double percent);
void stoploss_free(struct stoploss *s);

void stoploss_set(struct stoploss *s, stoploss_t type, double value);
void stoploss_clear(struct stoploss *s);

int stoploss_feed(struct indicator *i, const struct candle *candle);

#endif
