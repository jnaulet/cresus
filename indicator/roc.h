/*
 * Cresus EVO - roc.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 02.05.2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ROC_H
#define ROC_H

/*
 * Rate of Change
 * Momentum indicator
 * Typical period is 12
 */

#include "math/average.h"

#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

struct roc_n3 {
  /* As always... */
  struct indicator_n3 indicator_n3;
  /* Data */
  double value;
};

#define roc_n3_alloc(n3, parent, value)				\
  DEFINE_ALLOC(struct roc_n3, n3, roc_n3_init, parent, value)
#define roc_n3_free(n3)				\
  DEFINE_FREE(n3, roc_n3_free)

static inline int roc_n3_init(struct roc_n3 *ctx,
			      struct indicator *parent,
			      double value)
{
  indicator_n3_init(&ctx->indicator_n3, parent);
  ctx->value = value;
  return 0;
}

static inline void roc_n3_release(struct roc_n3 *ctx)
{
  indicator_n3_release(&ctx->indicator_n3);
}

/* Main object */

#define roc_alloc(ctx, uid, period, average)			\
  DEFINE_ALLOC(struct roc, ctx, roc_init, uid, period, average)
#define roc_free(ctx)				\
  DEFINE_FREE(ctx, roc_release)

struct roc {
  /* Inherits from indicator */
  struct indicator indicator;
  int period;
  struct average average;
};

int roc_init(struct roc *ctx, unique_id_t uid, int period, int average);
void roc_release(struct roc *ctx);

int roc_compute(struct roc *ctx, struct track_n3 *e, double *rvalue);
void roc_reset(struct roc *ctx);

#endif
