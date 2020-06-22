/*
 * Cresus Income Statement - income-statement.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/15/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef INCOME_STATEMENT_H
#define INCOME_STATEMENT_H

#include <string.h>
#include <limits.h>

#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/time64.h"

typedef enum {
  IS_SOMETHING = 0, IS_OTHER = 1 /* TBD */
} income_statement_n3_value_t;

/* Generic object */
struct income_statement_n3 {
  /* Inherits from list */
  struct list list;
  /* Internal data */
  time64_t time;
  union {
    struct {
      double value1;
      double value2;
    };
    double __income_statement_n3_value__[0];
  };
};

static inline int income_statement_n3_init(struct income_statement_n3 *ctx,
                                           time64_t time)
{
  list_init(&ctx->list); /* super() */
  return 0;
}

static inline void income_statement_n3_release(struct income_statement_n3 *ctx)
{
  list_release(&ctx->list);
}

#define income_statement_n3_alloc(ctx, time)				\
  DEFINE_ALLOC(struct income_statement_n3, ctx, income_statement_n3_init, time)
#define income_statement_n3_free(ctx)           \
  DEFINE_FREE(ctx, income_statement_n3_release)

/* typedefs */
struct income_statement;

struct income_statement_ops {
  int (*init)(struct income_statement *ctx);
  void (*release)(struct income_statement *ctx);
  struct income_statement_n3 *(*read)(struct income_statement *ctx);
};

struct income_statement {
  list_head_t(struct income_statement_n3) list_income_statement_n3s;
  char filename[256], ext[256];
  struct income_statement_ops *ops;
  void *private;
};

int income_statement_init(struct income_statement *ctx, const char *filename, const char *ext);
void income_statement_release(struct income_statement *ctx);

#define income_statement_alloc(ctx, filename, ext)                      \
  DEFINE_ALLOC(struct income_statement, ctx, income_statement_init, filename, ext)
#define income_statement_free(ctx)              \
  DEFINE_FREE(ctx, income_statement_release)

#endif
