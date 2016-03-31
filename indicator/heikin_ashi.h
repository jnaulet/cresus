//
//  heikin_ashi.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 26/01/2015.
//  Copyright (c) 2015 Joachim Naulet. All rights reserved.
//

#ifndef __Cresus_EVO__heikin_ashi__
#define __Cresus_EVO__heikin_ashi__

#include "framework/candle.h"
#include "framework/indicator.h"

typedef enum {
  HEIKIN_ASHI_DIR_NONE,
  HEIKIN_ASHI_DIR_UP,
  HEIKIN_ASHI_DIR_DOWN,
} heikin_ashi_dir_t;

struct heikin_ashi {
  struct indicator parent;
  
  struct candle value;
  heikin_ashi_dir_t dir;
};

int heikin_ashi_init(struct heikin_ashi *h, const struct candle *seed);
void heikin_ashi_free(struct heikin_ashi *h);

int heikin_ashi_feed(struct indicator *i, const struct candle *candle);

/* Indicator-specific */
int heikin_ashi_get(struct heikin_ashi *h, struct candle *candle);

#endif /* defined(__Cresus_EVO__heikin_ashi__) */
