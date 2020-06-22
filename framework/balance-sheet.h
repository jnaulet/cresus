/*
 * Cresus Balance Sheet - balance-sheet.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/11/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef BALANCE_SHEET_H
#define BALANCE_SHEET_H

#include <string.h>
#include <limits.h>

#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/time64.h"

typedef enum {
  BS_CASH = 0, BS_DEBT = 1 /* TBD */
} balance_sheet_n3_value_t;

/* Generic object */
struct balance_sheet_n3 {
  /* Inherits from list */
  struct list list;
  /* Internal data */
  time64_t time;
  union {
    struct {
      double value1;
      double value2;
    };
    double __balance_sheet_n3_value__[0];
  };
};

static inline int balance_sheet_n3_init(struct balance_sheet_n3 *ctx,
                                        time64_t time)
{
  list_init(&ctx->list); /* super() */
  return 0;
}

static inline void balance_sheet_n3_release(struct balance_sheet_n3 *ctx)
{
  list_release(&ctx->list);
}

#define balance_sheet_n3_alloc(ctx, time)				\
  DEFINE_ALLOC(struct balance_sheet_n3, ctx, balance_sheet_n3_init, time)
#define balance_sheet_n3_free(ctx)              \
  DEFINE_FREE(ctx, balance_sheet_n3_release)

/* typedefs */
struct balance_sheet;

struct balance_sheet_ops {
  int (*init)(struct balance_sheet *ctx);
  void (*release)(struct balance_sheet *ctx);
  struct balance_sheet_n3 *(*read)(struct balance_sheet *ctx);
};

struct balance_sheet {
  list_head_t(struct balance_sheet_n3) list_balance_sheet_n3s;
  char filename[256], ext[256];
  struct balance_sheet_ops *ops;
  void *private;
};

int balance_sheet_init(struct balance_sheet *ctx, const char *filename, const char *ext);
void balance_sheet_release(struct balance_sheet *ctx);

#define balance_sheet_alloc(ctx, filename, ext)                         \
  DEFINE_ALLOC(struct balance_sheet, ctx, balance_sheet_init, filename, ext)
#define balance_sheet_free(ctx)                 \
  DEFINE_FREE(ctx, balance_sheet_release)

#endif
