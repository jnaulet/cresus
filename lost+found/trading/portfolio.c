/*
 * Cresus EVO - portfolio.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "portfolio.h"

int portfolio_init(struct portfolio *p)
{
  p->index = PORTFOLIO_MAX;
  return 0;
}

void portfolio_release(struct portfolio *p)
{
}

double portfolio_buy(struct portfolio *p, portfolio_t type, double value)
{
  if(p->index <= 0)
    return 0.0;

  p->entry[--p->index].value = value;
  p->entry[p->index].type = type;

  return value;
}

double portfolio_sell(struct portfolio *p, double value)
{
  if(p->index >= PORTFOLIO_MAX)
    return 0.0;

  if(p->entry[p->index].type == PORTFOLIO_SHORT)
    return (p->entry[p->index++].value - value);

  return (value - p->entry[p->index++].value);
}

double portfolio_sell_all(struct portfolio *p, double value)
{
  double ret = 0.0;

  while(p->index < PORTFOLIO_MAX)
    ret += portfolio_sell(p, value);

  return ret;
}
