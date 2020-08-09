/*
 * Cresus EVO - portfolio.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 09.05.2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "portfolio.h"

int portfolio_init(struct portfolio *ctx)
{
  plist_head_init(&ctx->plist_portfolio_n3s);
  return 0;
}

void portfolio_release(struct portfolio *ctx)
{
  plist_head_release(&ctx->plist_portfolio_n3s);
}

static struct portfolio_n3 *
portfolio_find_n3(struct portfolio *ctx, unique_id_t uid)
{
  struct plist *p;
  plist_for_each(&ctx->plist_portfolio_n3s, p){
    struct portfolio_n3 *pos = p->ptr;
    if(pos->uid == uid)
      return pos;
  }
  
  return NULL;
}

double portfolio_add(struct portfolio *ctx,
		     const char *name, unique_id_t uid,
		     double shares, double price)
{
  struct portfolio_n3 *pos;
  if(!(pos = portfolio_find_n3(ctx, uid))){
    __try__(!portfolio_n3_alloc(pos, name, uid), err);
    plist_add_tail_ptr(&ctx->plist_portfolio_n3s, pos);
  }
  
  pos->cost_price = (pos->cost_price * pos->shares +
		     shares * price) / (pos->shares + shares);
  
  pos->shares += shares;
  pos->nbuy++;
  
  return (shares * price);
  
 __catch__(err):
  perror(__FUNCTION__);
  return 0.0;
}

double portfolio_sub(struct portfolio *ctx,
		     const char *name, unique_id_t uid,
		     double shares, double price)
{
  struct portfolio_n3 *pos;
  if(!(pos = portfolio_find_n3(ctx, uid)))
    return 0.0;
  
  double n = MIN(shares, pos->shares);
  
  pos->shares -= n;
  pos->nsell++;
  
  return n * price;
}
