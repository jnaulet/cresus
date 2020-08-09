/*
 * Cresus EVO - quotes.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/21/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef PRICE_H
#define PRICE_H

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/time64.h"

typedef enum {
  OPEN = 0, CLOSE = 1, HIGH = 2, LOW = 3, VOLUME = 4
} quotes_n3_value_t;

#define quotes_n3_value(ctx, value)              \
  ((ctx)->__quotes_n3_value__[value])

/* Price object format */

struct quotes_n3 {
  /* Inherits from list */
  struct list list;
  /* Internal data */
  time64_t time;
  union {
    struct { double open, close, high, low, volume; };
    double __quotes_n3_value__[0];
  };
};

#define quotes_n3_fmt				\
  "o%.2f c%.2f h%.2f l%.2f v%.0f"
#define quotes_n3_args(ctx)						\
  (ctx)->open, (ctx)->close, (ctx)->high, (ctx)->low, (ctx)->volume
#define quotes_n3_str(ctx, buf)		     \
  sprintf(buf, quotes_n3_interface_fmt,       \
          quotes_n3_interface_args(ctx))

static inline int quotes_n3_init(struct quotes_n3 *ctx,
                                time64_t time,
                                double open, double close,
                                double high, double low,
                                double volume)
{
  list_init(&ctx->list); /* super() */
  ctx->time = time;
  ctx->open = open;
  ctx->close = close;
  ctx->high = high;
  ctx->low = low;
  ctx->volume = volume;
  return 0;
}

static inline void quotes_n3_release(struct quotes_n3 *ctx)
{
  list_release(&ctx->list);
}

#define quotes_n3_alloc(ctx, time, open, close, high, low, volume) \
  DEFINE_ALLOC(struct quotes_n3, ctx, quotes_n3_init,               \
	       time, open, close, high, low, volume)
#define quotes_n3_free(ctx)			\
  DEFINE_FREE(ctx, quotes_n3_release)

/* Typedefs */
struct quotes;

struct quotes_ops {
  int (*init)(struct quotes *ctx);
  void (*release)(struct quotes *ctx);
  struct quotes_n3 *(*read)(struct quotes *ctx);
};

struct quotes {
  list_head_t(struct quotes_n3) list_quotes_n3s;
  char filename[256];
  struct quotes_ops *ops;
  void *private;
};

/*
 * Main quotes object
 */

int quotes_init(struct quotes *ctx, const char *filename);
void quotes_release(struct quotes *ctx);

#define quotes_alloc(ctx, filename)                             \
  DEFINE_ALLOC(struct quotes, ctx, quotes_init, filename)
#define quotes_free(ctx)                        \
  DEFINE_FREE(ctx, quotes_release)

#endif /* defined(PRICE_H) */
