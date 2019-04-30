/*
 * Cresus EVO - mobile.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef MOBILE_H
#define MOBILE_H

/*
 * Mobile Average
 * Exponential & Simple
 */

#include "math/average.h"
#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

/* TODO : Is that useful ? */
typedef enum {
  MOBILE_MMA,
  MOBILE_EMA
} mobile_t;

typedef enum {
  MOBILE_DIR_UP,
  MOBILE_DIR_DOWN
} mobile_dir_t;

/* Warning /!\
 * this is position of the mobile avg
 * compared to candle value
 * above means candle is below
 * and below means candle is above
 */
typedef enum {
  MOBILE_POS_ABOVE,
  MOBILE_POS_BELOW
} mobile_pos_t;

/* Indicator-specific events */
#define MOBILE_EVENT_CHDIR_UP     0
#define MOBILE_EVENT_CHDIR_DOWN   1
#define MOBILE_EVENT_CROSSED_DOWN 2
#define MOBILE_EVENT_CROSSED_UP   3

/* Timeline n3s object */

struct mobile_n3 {
  /* As below */
  __inherits_from__(struct indicator_n3);
  /* Single value */
  double value;
  /* More info */
  double direction;
  /* Events ? */
};

#define mobile_n3_alloc(ctx, parent, value, direction)		\
  DEFINE_ALLOC(struct mobile_n3, ctx,				\
	       mobile_n3_init, parent, value, direction)
#define mobile_n3_free(ctx)			\
  DEFINE_FREE(ctx, mobile_n3_release)

static inline int mobile_n3_init(struct mobile_n3 *ctx,
				    struct indicator *parent,
				    double value, double direction)
{
  __indicator_n3_init__(ctx, parent);
  ctx->value = value;
  ctx->direction = direction;
  return 0;
}

static inline void mobile_n3_release(struct mobile_n3 *ctx)
{
  __indicator_n3_release__(ctx);
}

/* Main object */

#define mobile_alloc(ctx, uid, type, period, value)			\
  DEFINE_ALLOC(struct mobile, ctx, mobile_init, uid, type, period, value)
#define mobile_free(ctx)                        \
  DEFINE_FREE(ctx, mobile_release)

struct mobile {
  /* As always */
  __inherits_from__(struct indicator);
  /* Basic data */
  mobile_t type;
  input_n3_value_t value;
  /* Average object */
  struct average avg;
};

int mobile_init(struct mobile *ctx, unique_id_t id, mobile_t type, int period, input_n3_value_t value);
void mobile_release(struct mobile *ctx);

/* indicator-specific */
double mobile_average(struct mobile *ctx);
double mobile_stddev(struct mobile *ctx);

#endif
