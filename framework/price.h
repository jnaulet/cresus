/*
 * Cresus EVO - price.h 
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
} price_n3_value_t;

#define price_n3_value(ctx, value)              \
  ((ctx)->__price_n3_value__[value])

/* Price object format */

struct price_n3 {
  /* Inherits from list */
  struct list list;
  /* Internal data */
  time64_t time;
  union {
    struct { double open, close, high, low, volume; };
    double __price_n3_value__[0];
  };
};

#define price_n3_fmt				\
  "o%.2f c%.2f h%.2f l%.2f v%.0f"
#define price_n3_args(ctx)						\
  (ctx)->open, (ctx)->close, (ctx)->high, (ctx)->low, (ctx)->volume
#define price_n3_str(ctx, buf)		     \
  sprintf(buf, price_n3_interface_fmt,       \
          price_n3_interface_args(ctx))

static inline int price_n3_init(struct price_n3 *ctx,
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

static inline void price_n3_release(struct price_n3 *ctx)
{
  list_release(&ctx->list);
}

#define price_n3_alloc(ctx, time, open, close, high, low, volume) \
  DEFINE_ALLOC(struct price_n3, ctx, price_n3_init,               \
	       time, open, close, high, low, volume)
#define price_n3_free(ctx)			\
  DEFINE_FREE(ctx, price_n3_release)

/* Typedefs */
struct price;

struct price_ops {
  int (*init)(struct price *ctx);
  void (*release)(struct price *ctx);
  struct price_n3 *(*read)(struct price *ctx);
};

struct price {
  list_head_t(struct price_n3) list_price_n3s;
  char filename[256], ext[256];
  struct price_ops *ops;
  void *private;
};

/*
 * Main price object
 */

int price_init(struct price *ctx, const char *filename, const char *ext);
void price_release(struct price *ctx);

#define price_alloc(ctx, filename, ext)                         \
  DEFINE_ALLOC(struct price, ctx, price_init, filename, ext)
#define price_free(ctx)                         \
  DEFINE_FREE(ctx, price_release)

#endif /* defined(PRICE_H) */
