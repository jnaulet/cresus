/*
 * Cresus EVO - position.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

//
//  position.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 28/01/2015.
//  Copyright (c) 2015 Joachim Naulet. All rights reserved.
//

#ifndef __Cresus_EVO__position__
#define __Cresus_EVO__position__

#include "framework/candle.h"

typedef enum {
  POSITION_NONE,
  POSITION_LONG,
  POSITION_SHORT
} position_t;

struct position {
  position_t type;
  double value;
  int number;
};

int position_init(struct position *p, position_t type,
                  const struct candle *candle,
                  candle_value_t value, int number);

void position_free(struct position *p);

double position_get_value(struct position *p,
                          const struct candle *candle,
                          candle_value_t value);

#endif /* defined(__Cresus_EVO__position__) */
