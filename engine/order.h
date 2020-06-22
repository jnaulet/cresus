/*
 * Cresus EVO - order.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 09.01.2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ORDER_H
#define ORDER_H

#include "engine/cert.h"
#include "framework/types.h"
#include "framework/alloc.h"

typedef enum {
  ORDER_BUY,
  ORDER_SELL,
  ORDER_SELL_ALL,
  ORDER_SELLSHORT,
  ORDER_EXITSHORT
} order_t;

typedef enum {
  ORDER_BY_NB,
  ORDER_BY_AMOUNT
} order_by_t;

/* Shortcuts */
#define BUY ORDER_BUY
#define SELL ORDER_SELL
#define SELL_ALL ORDER_SELL_ALL
#define SELLSHORT ORDER_SELLSHORT
#define EXITSHORT ORDER_EXITSHORT

#define BY_NB  ORDER_BY_NB
#define SHARES ORDER_BY_NB
#define BY_AMOUNT ORDER_BY_AMOUNT
#define CASH      ORDER_BY_AMOUNT

#define order_alloc(ctx, type, by, value, cert)		\
  DEFINE_ALLOC(struct order, ctx, order_init,		\
	       type, by, value, (cert))
#define order_free(ctx)				\
  DEFINE_FREE(ctx, order_release)

struct order {
  /* Private data */
  order_t type;
  order_by_t by;
  double value;
  /* Certificates */
  struct cert cert;
};

int order_init(struct order *ctx, order_t t, order_by_t by,
	       double value, struct cert *cert);
void order_release(struct order *ctx);

const char *order_str(struct order *ctx);
const char *order_str_r(struct order *ctx, char *buf);

#endif
