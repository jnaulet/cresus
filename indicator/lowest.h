/*
 * Cresus EVO - lowest.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08.05.2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef LOWEST_H
#define LOWEST_H

/*
 * Lowest
 * Some support indicator
 */

#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

struct lowest_n3 {
  /* As always... */
  struct indicator_n3 indicator_n3;
  /* Data */
  double value;
};

#define lowest_n3_alloc(ctx, parent)				\
  DEFINE_ALLOC(struct lowest_n3, ctx, lowest_n3_init, parent)
#define lowest_n3_free(ctx)			\
  DEFINE_FREE(ctx, lowest_n3_free)

static inline int lowest_n3_init(struct lowest_n3 *ctx,
				 struct indicator *parent)
{
  indicator_n3_init(&ctx->indicator_n3, parent);
  return 0;
}

static inline void lowest_n3_release(struct lowest_n3 *ctx)
{
  indicator_n3_release(&ctx->indicator_n3);
}

/* Main object */

#define lowest_alloc(ctx, uid, period)				\
  DEFINE_ALLOC(struct lowest, ctx, lowest_init, uid, period)
#define lowest_free(ctx)			\
  DEFINE_FREE(ctx, lowest_release)

struct lowest {
  /* Inherits from indicator */
  struct indicator indicator;
  int period;
};

int lowest_init(struct lowest *ctx, unique_id_t uid, int period);
void lowest_release(struct lowest *ctx);

void lowest_reset(struct lowest *ctx);

#endif
