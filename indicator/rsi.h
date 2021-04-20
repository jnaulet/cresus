/*
 * Cresus EVO - rsi.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef RSI_H
#define RSI_H

/*
 * Relative Strength Index
 */

#include "math/average.h"

#include "framework/list.h"
#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

/*
 * Entry
 */

struct rsi_n3 {
  /* As always... */
  struct indicator_n3 indicator_n3;
  /* Data */
  double value;
};

#define rsi_n3_alloc(ctx, parent, value)                        \
  DEFINE_ALLOC(struct rsi_n3, ctx, rsi_n3_init, parent, value)
#define rsi_n3_free(ctx)                        \
  DEFINE_FREE(ctx, rsi_n3_free)

static inline int rsi_n3_init(struct rsi_n3 *ctx,
                              struct indicator *parent,
                              double value)
{
  indicator_n3_init(&ctx->indicator_n3, parent);
  ctx->value = value;
  return 0;
}

static inline void rsi_n3_release(struct rsi_n3 *ctx)
{
  indicator_n3_release(&ctx->indicator_n3);
}

/*
 * Main object
 */

#define rsi_alloc(ctx, uid, period)                     \
  DEFINE_ALLOC(struct rsi, ctx, rsi_init, uid, period)
#define rsi_free(ctx)                           \
  DEFINE_FREE(ctx, rsi_release)

struct rsi {
  /* Inherits from indicator */
  struct indicator indicator;  
  /* https://fr.wikipedia.org/wiki/Relative_strength_index */
  struct average h;
  struct average b;
  int period;
};

int rsi_init(struct rsi *ctx, unique_id_t uid, int period);
void rsi_release(struct rsi *ctx);

#endif
