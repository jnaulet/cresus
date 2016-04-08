/*
 * Cresus EVO - heikin_ashi.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 01/26/2015
 * Copyright (c) 2015 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__heikin_ashi__
#define __Cresus_EVO__heikin_ashi__

#include "engine/candle.h"
#include "framework/indicator.h"

typedef enum {
  HEIKIN_ASHI_DIR_NONE,
  HEIKIN_ASHI_DIR_UP,
  HEIKIN_ASHI_DIR_DOWN,
} heikin_ashi_dir_t;

#define HEIKIN_ASHI_EVENT_CHDIR_UP   0
#define HEIKIN_ASHI_EVENT_CHDIR_DOWN 1

struct heikin_ashi {
  __inherits_from_indicator__;

  struct candle value;
  candle_value_t cvalue;
  
  heikin_ashi_dir_t dir;
};

int heikin_ashi_init(struct heikin_ashi *h, indicator_id_t id);
void heikin_ashi_release(struct heikin_ashi *h);

/* Indicator-specific */
int heikin_ashi_get(struct heikin_ashi *h, struct candle *candle);

#endif /* defined(__Cresus_EVO__heikin_ashi__) */
