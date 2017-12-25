/*
 * Cresus EVO - order.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 09.01.2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "order.h"

int order_init(struct order *ctx, order_t type, order_by_t by, double value)
{
  /* super() */
  __list_super__(ctx);
  
  ctx->type = type;
  ctx->by = by;
  ctx->value = value;
  
  return 0;
}

void order_release(struct order *ctx)
{
  __list_release__(ctx);
}

static char buf[256];

const char *order_str(struct order *ctx)
{
  return order_str_r(ctx, buf);
}

static const char *order_str_type(struct order *ctx)
{
  switch(ctx->type){
  case ORDER_BUY: return "BUY";
  case ORDER_SELL: return "SELL";
  case ORDER_SELLSHORT: return "SELLSHORT";
  case ORDER_EXITSHORT: return "EXITSHORT";
  }
  
  return NULL;
}

static const char *order_str_by(struct order *ctx)
{
  switch(ctx->by){
  case ORDER_BY_NB: return "STOCK";
  case ORDER_BY_AMOUNT: return "CASH";
  }

  return NULL;
}

const char *order_str_r(struct order *ctx, char *buf)
{
  sprintf(buf, "%s %.2lf %s", order_str_type(ctx), ctx->value,
	  order_str_by(ctx));
  
  return buf;
}
