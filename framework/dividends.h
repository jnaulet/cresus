/*
 * Cresus - dividends.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/25/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef DIVIDENDS_H
#define DIVIDENDS_H

#include "framework/time.h"
#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/types.h"

struct dividends_n3 {
  /* Inherits from list */
  struct list list;
  /* Time */
  time_t time;
  /* Data */
  double per_share;
};

static inline int dividends_n3_init(struct dividends_n3 *ctx,
				    time_t time, double per_share)
{
  list_init(&ctx->list); /* parent() */
  ctx->time = time;
  ctx->per_share = per_share;
  return 0;
}

static inline void dividends_n3_release(struct dividends_n3 *ctx)
{
  list_release(&ctx->list);
}

#define dividends_n3_alloc(ctx, time, per_share)			\
  DEFINE_ALLOC(struct dividends_n3, ctx, dividends_n3_init, time, per_share)
#define dividends_n3_free(ctx)			\
  DEFINE_FREE(ctx, dividends_n3_release)

struct dividends;

struct dividends_ops {
  int (*init)(struct dividends *ctx);
  void (*release)(struct dividends *ctx);
  struct dividends_n3 *(*read)(struct dividends *ctx);
};

struct dividends {
  list_head_t(struct dividends_n3) list_dividends_n3s;
  char filename[256], ext[256];
  struct dividends_ops *ops;
  void *private;
};

int dividends_init(struct dividends *ctx, const char *filename);
void dividends_release(struct dividends *ctx);

#define dividends_alloc(ctx, filename)				\
  DEFINE_ALLOC(struct dividends, ctx, dividends_init, filename)
#define dividends_free(ctx)			\
  DEFINE_FREE(ctx, dividends_release)

#endif
