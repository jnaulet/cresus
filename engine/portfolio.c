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
portfolio_find_n3(struct portfolio *ctx, unique_id_t uid,
		  unique_id_t leveraged_uid)
{
  struct plist *p;
  
  plist_for_each(&ctx->plist_portfolio_n3s, p){
    struct portfolio_n3 *pos = p->ptr;
    if(pos->uid == uid && pos->leveraged_uid == leveraged_uid)
      return pos;
  }
  
  return NULL;
}

double portfolio_add(struct portfolio *ctx,
		     const char *name, unique_id_t uid,
		     double shares, double price)
{
  struct portfolio_n3 *pos;
  if(!(pos = portfolio_find_n3(ctx, uid, 0))){
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

double portfolio_add_leveraged(struct portfolio *ctx,
			       const char *name, unique_id_t uid,
			       double shares, double price,
			       double funding, double ratio,
			       double stoploss)
{
  struct portfolio_n3 *pos;
  unique_id_t leveraged_uid = portfolio_n3_leveraged_uid(uid, funding);
  
  if(!(pos = portfolio_find_n3(ctx, uid, leveraged_uid))){
    __try__(!portfolio_n3_alloc(pos, name, uid), err);
    plist_add_tail_ptr(&ctx->plist_portfolio_n3s, pos);
    /* Leverage info */
    portfolio_n3_set_leveraged(pos, funding, ratio, stoploss);
  }
  
  price -= funding;
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
  if(!(pos = portfolio_find_n3(ctx, uid, 0)))
    return 0.0;
  
  double n = MIN(shares, pos->shares);
  
  pos->shares -= n;
  pos->nsell++;
  
  return n * price;
}

int portfolio_run(struct portfolio *ctx, double low)
{
  struct plist *p, *safe;
  
  plist_for_each_safe(&ctx->plist_portfolio_n3s, p, safe){
    struct portfolio_n3 *pos = p->ptr;
    /* Check if pos <= 0 */
    if(low <= pos->stoploss){
      PR_ERR("%s stoploss hit at %.2lf !!!\n", pos->name, low);
      plist_del(p);
      portfolio_n3_free(pos);
    }
  }
  
  return 0;
}
