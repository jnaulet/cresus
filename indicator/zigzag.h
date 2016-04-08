/*
 * Cresus EVO - zigzag.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ZIGZAG_H
#define ZIGZAG_H

#include "engine/candle.h"
#include "framework/indicator.h"

typedef enum {
   ZIGZAG_PERCENT,
   ZIGZAG_ABSOLUTE
} zigzag_t;

typedef enum {
  ZIGZAG_DIR_NONE,
  ZIGZAG_DIR_UP,
  ZIGZAG_DIR_DOWN
} zigzag_dir_t;

typedef enum {
  ZIGZAG_REF_HIGH,
  ZIGZAG_REF_LOW,
  ZIGZAG_REF_CLOSE,
  ZIGZAG_REF_OPEN,
  ZIGZAG_REF_TYPICAL
} zigzag_ref_t;

#define ZIGZAG_EVENT_CHDIR_UP   0
#define ZIGZAG_EVENT_CHDIR_DOWN 1

struct zigzag {
  
  __inherits_from_indicator__;

  zigzag_t type;
  double threshold;
  candle_value_t cvalue;
  
  int ref_count; /* Candles since last ref */
  struct candle *ref, *last_ref;

  zigzag_dir_t direction; /* FIXME : use int ? */
};

int zigzag_init(struct zigzag *z, indicator_id_t id, zigzag_t type,
		double thres, candle_value_t cvalue);
void zigzag_release(struct zigzag *z);

/* Indicator-specific */
zigzag_dir_t zigzag_get_last_ref(struct zigzag *z, struct candle *last_ref);

#endif
