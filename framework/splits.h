/*
 * Cresus - splits.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/25/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef SPLITS_H
#define SPLITS_H

#include "framework/time.h"
#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/types.h"

struct splits_n3 {
  /* Inherits from list */
  struct list list;
  /* Time */
  time_t time;
  /* Data */
  double fact;
  double denm;
};

static inline int splits_n3_init(struct splits_n3 *ctx, time_t time,
				 double fact, double denm)
{
  list_init(&ctx->list); /* parent() */
  ctx->time = time;
  ctx->fact = fact;
  ctx->denm = denm;
  return 0;
}

static inline void splits_n3_release(struct splits_n3 *ctx)
{
  list_release(&ctx->list);
}

#define splits_n3_alloc(ctx, time, fact, denm)				\
  DEFINE_ALLOC(struct splits_n3, ctx, splits_n3_init, time, fact, denm)
#define splits_n3_free(ctx)			\
  DEFINE_FREE(ctx, splits_n3_release)

struct splits;

struct splits_ops {
  int (*init)(struct splits *ctx);
  void (*release)(struct splits *ctx);
  struct splits_n3 *(*read)(struct splits *ctx);
};

struct splits {
  list_head_t(struct splits_n3) list_splits_n3s;
  char filename[256], ext[256];
  struct splits_ops *ops;
  void *private;
};

int splits_init(struct splits *ctx, const char *filename);
void splits_release(struct splits *ctx);

#define splits_alloc(ctx, filename)				\
  DEFINE_ALLOC(struct splits, ctx, splits_init, filename)
#define splits_free(ctx)			\
  DEFINE_FREE(ctx, splits_release)

#endif
