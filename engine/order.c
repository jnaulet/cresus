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
}
