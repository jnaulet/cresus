/*
 * Cresus EVO - portfolio.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 09.05.2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <string.h>

#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/types.h"
#include "framework/verbose.h"

/*
 * Data hierarchy
 *
 * Portfolio
 * |- n3 #0 -> n3 #1 -> n3 #n
 *
 */

/*
 * Positions / portfolio entries
 */

typedef enum {
   PORTFOLIO_N3_STANDARD,
   PORTFOLIO_N3_LEVERAGED
} portfolio_n3_t;

struct portfolio_n3 {
  __inherits_from__(struct list);
  /* Common */
  char name[64]; /* TODO : use macro */
  unique_id_t uid;
  portfolio_n3_t type;
  /* Standard */
  double shares, cost_price;
  /* Leveraged */
  unique_id_t leveraged_uid;
  double funding, ratio, stoploss;
  /* Statistics */
  int nbuy, nsell;
};

static inline int
portfolio_n3_init(struct portfolio_n3 *ctx,
		  const char *name, unique_id_t uid)
{
  __list_init__(ctx);
  ctx->uid = uid;
  ctx->shares = 0.0;
  ctx->cost_price = 0.0;
  ctx->type = PORTFOLIO_N3_STANDARD;
  ctx->leveraged_uid = 0; /* FIXME */
  strncpy(ctx->name, name, sizeof(ctx->name));
  ctx->nbuy = 0;
  ctx->nsell = 0;
  return 0;
}

#define portfolio_n3_alloc(ctx, name, uid)	\
  DEFINE_ALLOC(struct portfolio_n3, ctx,	\
	       portfolio_n3_init, name, uid)
#define portfolio_n3_free(ctx)			\
  DEFINE_FREE(ctx, release_dummy)

#define portfolio_n3_leveraged_uid(uid, funding)	\
  (((uid) << 16) | (int)funding)

static inline void
portfolio_n3_set_leveraged(struct portfolio_n3 *ctx,
			   double funding, double ratio,
			   double stoploss)
{
  ctx->type = PORTFOLIO_N3_LEVERAGED;
  ctx->funding = funding;
  ctx->ratio = ratio;
  ctx->stoploss = stoploss;
  ctx->leveraged_uid = portfolio_n3_leveraged_uid(ctx->uid, funding);
  sprintf(ctx->name + strlen(ctx->name), "_%.0lf", funding);
}

/* Stats */
#define portfolio_n3_price(ctx, price)		\
  (price - (ctx->funding))
#define portfolio_n3_pvalue(ctx, price)					\
  ((ctx)->shares * (portfolio_n3_price(ctx, price) - (ctx)->cost_price))
#define portfolio_n3_total_cost(ctx)		\
  ((ctx)->shares * (ctx)->cost_price)
#define portfolio_n3_total_value(ctx, price)		\
  ((ctx)->shares * (portfolio_n3_price(ctx, price)))
#define portfolio_n3_performance(ctx, price)	\
  ((portfolio_n3_total_value(ctx, price) /	\
    portfolio_n3_total_cost(ctx)) - 1.0)

static inline void
portfolio_n3_pr_stat(struct portfolio_n3 *ctx,
		     double price)
{
  PR_STAT("%s [price] %.2lf [cost price] %.2lf [cost] %.2lf "
	  "[+/-] %.2lf [total] %.2lf [performance] %.2lf%% "
	  "[nbuy] %d [nsell] %d\n",
	  ctx->name, price, ctx->cost_price,
	  portfolio_n3_total_cost(ctx),
	  portfolio_n3_pvalue(ctx, price),
	  portfolio_n3_total_value(ctx, price),
	  portfolio_n3_performance(ctx, price) * 100.0,
	  ctx->nbuy, ctx->nsell);
}

/*
 * Main object
 */

struct portfolio {
  list_head_t(struct portfolio_n3) list_portfolio_n3s;
};

/* Init */
int portfolio_init(struct portfolio *ctx);
void portfolio_release(struct portfolio *ctx);

/* Methods */
double portfolio_add(struct portfolio *ctx, const char *name, unique_id_t uid, double shares, double price);
double portfolio_add_leveraged(struct portfolio *ctx, const char *name, unique_id_t uid, double shares, double price, double funding, double ratio, double stoploss);
double portfolio_sub(struct portfolio *ctx, const char *name, unique_id_t uid, double shares, double price);

/* TODO : add more arguments */
int portfolio_run(struct portfolio *ctx, double low);

#endif
