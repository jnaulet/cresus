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

#include "framework/alloc.h"
#include "framework/types.h"
#include "framework/plist.h"
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

struct portfolio_n3 {
  /* Common */
  char name[64]; /* TODO : use macro */
  unique_id_t uid;
  /* Standard */
  double shares, cost_price;
  double dividends;
  /* Statistics */
  int nbuy, nsell;
};

static inline int
portfolio_n3_init(struct portfolio_n3 *ctx,
		  const char *name, unique_id_t uid)
{
  ctx->uid = uid;
  ctx->shares = 0.0;
  ctx->cost_price = 0.0;
  ctx->dividends = 0.0;
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

/* Stats */
#define portfolio_n3_total_cost(ctx)		\
  ((ctx)->shares * (ctx)->cost_price)
#define portfolio_n3_pmvalue(ctx, price)        \
  ((ctx)->shares * (price) - (ctx)->cost_price)
#define portfolio_n3_dividends(ctx)             \
  ((ctx)->dividends)
#define portfolio_n3_total_value(ctx, price)	\
  ((ctx)->shares * (price) + (ctx)->dividends)
#define portfolio_n3_performance(ctx, price)	\
  ((portfolio_n3_total_value(ctx, price) /	\
    portfolio_n3_total_cost(ctx)) - 1.0)

static inline void
portfolio_n3_pr_stat(struct portfolio_n3 *ctx,
		     double price)
{
  PR_STAT("%s [price] %.2lf [cost price] %.2lf [cost] %.2lf "   \
	  "[+/-] %.2lf [dividends] %.2lf [total] %.2lf "        \
          "[performance] %.2lf%% [nbuy] %d [nsell] %d\n",
	  ctx->name, price, ctx->cost_price,
	  portfolio_n3_total_cost(ctx),
	  portfolio_n3_pmvalue(ctx, price),
          portfolio_n3_dividends(ctx),
          portfolio_n3_total_value(ctx, price),
          portfolio_n3_performance(ctx, price) * 100.0,
	  ctx->nbuy, ctx->nsell);
}

/*
 * Main object
 */

struct portfolio {
  plist_head_t(struct portfolio_n3) plist_portfolio_n3s;
};

/* Init */
int portfolio_init(struct portfolio *ctx);
void portfolio_release(struct portfolio *ctx);

/* Methods */
double portfolio_add(struct portfolio *ctx, const char *name, unique_id_t uid, double shares, double price);
double portfolio_sub(struct portfolio *ctx, const char *name, unique_id_t uid, double shares, double price);
double portfolio_add_split(struct portfolio *ctx, const char *name, unique_id_t uid, double fact, double denm);
double portfolio_add_dividends_per_share(struct portfolio *ctx, const char *name, unique_id_t uid, double value);

#endif
