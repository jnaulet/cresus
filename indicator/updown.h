/*
 * Cresus EVO - updown.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/10/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef UPDOWN_H
#define UPDOWN_H

#include "engine/candle.h"
#include "framework/indicator.h"

#define UPDOWN_MAX 3

typedef enum {
  UPDOWN_NONE,
  UPDOWN_TOP,
  UPDOWN_BOTTOM
} updown_t;

struct updown {
  /* This is an indicator */
  __inherits_from_indicator__;
  /* Own data */
  updown_t type;
  candle_value_t cvalue;
};

int updown_init(struct updown *u, indicator_id_t id, candle_value_t cvalue);
void updown_release(struct updown *u);

/* Indicator-specific */
updown_t updown_type_get(struct updown *u);

#endif
