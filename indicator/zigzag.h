/*
 * Cresus EVO - zigzag.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ZIGZAG_H
#define ZIGZAG_H

#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

/* Common */

typedef enum {
  ZIGZAG_NONE,
  ZIGZAG_UP,
  ZIGZAG_DOWN,
} zigzag_dir_t;

/* Entries */

#define zigzag_n3_alloc(ctx, parent, dir, value, count)	\
  DEFINE_ALLOC(struct zigzag_n3, ctx,			\
	       zigzag_n3_init, parent, dir, value, count)
#define zigzag_n3_free(ctx)                  \
  DEFINE_FREE(ctx, zigzag_n3_release)

struct zigzag_n3 {
  /* As always */
  struct indicator_n3 indicator_n3;
  /* Self */
  double value;
  zigzag_dir_t dir;
  int n_since_last_ref;
  struct track_n3 *ref;
};

static inline int zigzag_n3_init(struct zigzag_n3 *ctx,
				    struct indicator *parent,
				    zigzag_dir_t dir, double value,
				    int count)
{
  indicator_n3_init(&ctx->indicator_n3, parent);
  ctx->dir = dir;
  ctx->value = value;
  ctx->n_since_last_ref = count;
  return 0;
}

static inline void zigzag_n3_release(struct zigzag_n3 *ctx)
{
  indicator_n3_release(&ctx->indicator_n3);
}

/* Indicator */

#define zigzag_alloc(ctx, uid, thres, value)				\
  DEFINE_ALLOC(struct zigzag, ctx, zigzag_init, uid, thres, value)
#define zigzag_free(ctx)                        \
  DEFINE_FREE(ctx, zigzag_release)

struct zigzag {
  /* As usual */
  struct indicator indicator;
  /* Params */
  double threshold;
  quotes_n3_value_t value;
  /* Internals */
  zigzag_dir_t dir;
  /* Some other stuff */
  int ref_count; /* Candles since base ref */
  struct track_n3 *ref, *base_ref;
};

int zigzag_init(struct zigzag *ctx, unique_id_t uid, double threshold, quotes_n3_value_t value);
void zigzag_release(struct zigzag *ctx);

#endif
