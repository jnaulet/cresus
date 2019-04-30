/*
 * Cresus EVO - swing.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef SWING_H
#define SWING_H

#include "engine/candle.h"
#include "framework/types.h"
#include "framework/indicator.h"

#define SWING_MAX 3

typedef enum {
  SWING_NONE,
  SWING_TOP,
  SWING_BOTTOM
} swing_t;

struct swing {
  /* This is an indicator */
  __inherits_from__(struct indicator);
  /* Own data */
  swing_t type;
  struct candle *ref;
};

int swing_init(struct swing *s, unique_id_t id);
void swing_release(struct swing *s);

/* Indicator-specific */
swing_t swing_type_get(struct swing *s);

#endif
