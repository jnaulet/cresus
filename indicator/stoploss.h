/*
 * Cresus EVO - stoploss.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/17/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef STOPLOSS_H
#define STOPLOSS_H

#include "framework/indicator.h"

typedef enum {
  STOPLOSS_UP,
  STOPLOSS_DOWN
} stoploss_t;

/* Events */
#define STOPLOSS_EVENT_HIT 0

struct stoploss {
  /* Inherits from indicator */
  __inherits_from_indicator__;
  
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

#endif
