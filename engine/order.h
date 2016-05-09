/*
 * Cresus EVO - order.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 09.01.2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ORDER_H
#define ORDER_H

#include "framework/list.h"
#include "framework/alloc.h"

typedef enum {
  ORDER_BUY,
  ORDER_SELL,
  ORDER_SELLSHORT,
  ORDER_EXITSHORT
} order_t;

typedef enum {
  ORDER_BY_NB,
  ORDER_BY_AMOUNT
} order_by_t;

#define order_alloc(ctx, type, by, value)			\
  DEFINE_ALLOC(struct order, ctx, order_init, type, by, value)
#define order_free(ctx)				\
  DEFINE_FREE(ctx, order_release)

struct order {
  /* listable */
  __inherits_from_list__;
  /* Private data */
  order_t type;
  order_by_t by;
  double value;
};

int order_init(struct order *ctx, order_t type, order_by_t by, double value);
void order_release(struct order *ctx);

#endif
