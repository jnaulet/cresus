/*
 * Cresus EVO - linear_reg.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 17/03/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef LINEAR_REG_H
#define LINEAR_REG_H

/*
 * Linear regression
 */

#include "math/average.h"
#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

/* Timeline n3s object */

struct linear_reg_n3 {
  /* As below */
  struct indicator_n3 indicator_n3;
  /* Single value */
  double value; /* End value for us */
  /* debug */
  double a, b; /* As in Y = aX + b */
};

#define linear_reg_n3_alloc(ctx, parent, value)			\
  DEFINE_ALLOC(struct linear_reg_n3, ctx,			\
	       linear_reg_n3_init, parent, value)
#define linear_reg_n3_free(ctx)			\
  DEFINE_FREE(ctx, linear_reg_n3_release)

static inline int linear_reg_n3_init(struct linear_reg_n3 *ctx,
				     struct indicator *parent,
				     double value)
{
  indicator_n3_init(&ctx->indicator_n3, parent);
  ctx->value = value;
  return 0;
}

static inline void linear_reg_n3_release(struct linear_reg_n3 *ctx)
{
  indicator_n3_release(&ctx->indicator_n3);
}

/* Main object */

#define linear_reg_alloc(ctx, uid, period)                              \
  DEFINE_ALLOC(struct linear_reg, ctx, linear_reg_init, uid, period)
#define linear_reg_free(ctx)			\
  DEFINE_FREE(ctx, linear_reg_release)

struct linear_reg {
  /* As always */
  struct indicator indicator;
  /* Internals */
  int period;
};

int linear_reg_init(struct linear_reg *ctx, unique_id_t id, int period);
void linear_reg_release(struct linear_reg *ctx);

#endif
