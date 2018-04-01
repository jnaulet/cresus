/*
 * Cresus EVO - types.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 13/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef TYPES_H
#define TYPES_H

#define BITMASK(n) ((1ll << n) - 1)
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Trend */

typedef enum {
  TREND_NONE,
  TREND_UP,
  TREND_DOWN
} trend_t;

#endif
