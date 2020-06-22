/*
 * Cresus Income Statement - income-statement.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/15/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef CASH_FLOW_H
#define CASH_FLOW_H

#include <string.h>
#include <limits.h>

#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/time64.h"

typedef enum {
  CF_SOMETHING = 0, CF_OTHER = 1 /* TBD */
} cash_flow_n3_value_t;

/* Generic object */
struct cash_flow_n3 {
  /* Inherits from list */
  struct list list;
  /* Internal data */
  time64_t time;
  union {
    struct {
      double value1;
      double value2;
    };
    double __cash_flow_n3_value__[0];
  };
};

static inline int cash_flow_n3_init(struct cash_flow_n3 *ctx,
                                    time64_t time)
{
  list_init(&ctx->list); /* super() */
  return 0;
}

static inline void cash_flow_n3_release(struct cash_flow_n3 *ctx)
{
  list_release(&ctx->list);
}

#define cash_flow_n3_alloc(ctx, time)                                   \
  DEFINE_ALLOC(struct cash_flow_n3, ctx, cash_flow_n3_init, time)
#define cash_flow_n3_free(ctx)                  \
  DEFINE_FREE(ctx, cash_flow_n3_release)

/* typedefs */
struct cash_flow;

struct cash_flow_ops {
  int (*init)(struct cash_flow *ctx);
  void (*release)(struct cash_flow *ctx);
  struct cash_flow_n3 *(*read)(struct cash_flow *ctx);
};

struct cash_flow {
  list_head_t(struct cash_flow_n3) list_cash_flow_n3s;
  char filename[256], ext[256];
  struct cash_flow_ops *ops;
  void *private;
};

int cash_flow_init(struct cash_flow *ctx, const char *filename, const char *ext);
void cash_flow_release(struct cash_flow *ctx);

#define cash_flow_alloc(ctx, filename, ext)                             \
  DEFINE_ALLOC(struct cash_flow, ctx, cash_flow_init, filename, ext)
#define cash_flow_free(ctx)                     \
  DEFINE_FREE(ctx, cash_flow_release)

#endif
