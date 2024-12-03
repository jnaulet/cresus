/*
 * Cresus EVO - bruss.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef BRUSS_H
#define BRUSS_H

/*
 * Bruss Average
 * Exponential & Simple
 */

#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

/* Timeline n3s object */

struct bruss_n3 {
  /* As below */
  struct indicator_n3 indicator_n3;
  /* Single value */
  double high;
  double low;
};

#define bruss_n3_alloc(ctx, parent, value, direction)		\
  DEFINE_ALLOC(struct bruss_n3, ctx,				\
	       bruss_n3_init, parent, value, direction)
#define bruss_n3_free(ctx)			\
  DEFINE_FREE(ctx, bruss_n3_release)

static inline int bruss_n3_init(struct bruss_n3 *ctx,
				 struct indicator *parent,
				double high, double low)
{
  indicator_n3_init(&ctx->indicator_n3, parent);
  ctx->high = high;
  ctx->low = low;
  return 0;
}

static inline void bruss_n3_release(struct bruss_n3 *ctx)
{
  indicator_n3_release(&ctx->indicator_n3);
}

/* Main object */

#define bruss_alloc(ctx, uid, period)				\
  DEFINE_ALLOC(struct bruss, ctx, bruss_init, uid, period)
#define bruss_free(ctx)                        \
  DEFINE_FREE(ctx, bruss_release)

struct bruss {
  /* As always */
  struct indicator indicator;
  /* Basic data */
  int period;
  int sample;
};

int bruss_init(struct bruss *ctx, unique_id_t id, int period);
void bruss_release(struct bruss *ctx);

#endif
