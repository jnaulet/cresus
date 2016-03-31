//
//  position.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 28/01/2015.
//  Copyright (c) 2015 Joachim Naulet. All rights reserved.
//

#include "position.h"

int position_init(struct position *p, position_t type,
                  const struct candle *candle,
                  candle_value_t value, int number) {
  
  p->type = type;
  p->value = (candle ? candle_get_value(candle, value) : 0.0);
  p->number = number;
  
  return 0;
}

void position_free(struct position *p) {
  
  p->type = POSITION_NONE;
  p->value = 0.0;
  p->number = 0;
}

double position_get_value(struct position *p,
                          const struct candle *candle,
                          candle_value_t value) {
  
  double current = candle_get_value(candle, value);
  
  if(p->type == POSITION_LONG)
    return current - p->value;
  
  if(p->type == POSITION_SHORT)
    return p->value - current;
  
  return 0.0;
}