/*
 * Cresus v2 - metrics.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/17/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "metrics.h"

#define metrics(a, b)                                                   \
  ({ while((a)->b == 0.0){                                              \
      const typeof(a) p = (void*)list_prev_safe(&(a)->list);            \
      if((a) != p) a = p; else break; } (a)->b; })

static void metrics_feed_anyway(struct metrics_n3 *ctx,
				struct balance_sheet_n3 *b,
				double net_income, double ebit)
{
  double inventory = metrics(b, inventory);
  double total_liab = metrics(b, total_liab);
  double total_assets = metrics(b, total_assets);
  double total_current_assets = metrics(b, total_current_assets);
  double total_stockholder_equity = metrics(b, total_stockholder_equity);
  double total_current_liabilities = metrics(b, total_current_liabilities);
  
  ctx->pe = ctx->market_cap / net_income;
  ctx->pbv = ctx->market_cap / ctx->book_value;
  ctx->eps = net_income / ctx->shares;
  ctx->roe = net_income / total_stockholder_equity;
  ctx->roce = ebit / (total_assets - total_current_liabilities);
  ctx->bvps = ctx->book_value / ctx->shares;
  ctx->cr = total_current_assets / total_current_liabilities;
  ctx->qr = (total_current_assets - inventory) / total_current_liabilities;
  ctx->de = total_liab / total_stockholder_equity;
}

static void metrics_feed_yearly(struct metrics_n3 *ctx,
                                struct balance_sheet_n3 *b,
                                struct income_statement_n3 *i)
{
  metrics_feed_anyway(ctx, b, i->net_income, i->ebit);
}

static void metrics_feed_quarterly(struct metrics_n3 *ctx,
				   struct balance_sheet_n3 *b,
				   struct income_statement_n3 *i)
{
  double ebit = 0.0;
  double net_income = 0.0;
  
  /* Last 4 quarters */
  for(int j = 4; j--;){
    ebit += i->ebit;
    net_income += i->net_income;
    i = (void*)list_prev_safe(&i->list);
  }
  
  metrics_feed_anyway(ctx, b, net_income, ebit);
}

static int metrics_feed(struct indicator *i, struct track_n3 *t)
{
  struct metrics_n3 *n3;
  struct metrics *ctx = (void*)i;

  struct balance_sheet_n3 *b = NULL;

  struct quotes_n3 *q = t->quotes;
  struct balance_sheet_n3 *by = t->balance_sheet.yearly;
  struct balance_sheet_n3 *bq = t->balance_sheet.quarterly;
  struct income_statement_n3 *iy = t->income_statement.yearly;
  struct income_statement_n3 *iq = t->income_statement.quarterly;
  
  /* Sort by most recent data */
  if(!metrics_n3_alloc(n3, i))
    return 0;

  /*
   * Balance sheet
   */
  
  /* 1st : quarterly */
  if(bq) b = bq;
  /* 2nd : yearly */
  else if(by) b = by;
  /* No releant data */
  else goto err;

  n3->shares = metrics(b, common_stock_shares_outstanding);
  n3->market_cap = n3->shares * q->close;
  n3->working_cap = (metrics(b, total_current_assets) -
                     metrics(b, total_current_liabilities));
  n3->book_value = (metrics(b, total_assets) -
                    metrics(b, intangible_assets) -
                    metrics(b, total_liab));
  
  /*
   * Income
   */
  
  /* 1st : quarterly */
  if(iq) metrics_feed_quarterly(n3, b, iq);
  /* 2nd : yearly */
  else if(iy) metrics_feed_yearly(n3, b, iy);
  /* No relevant data */
  else goto err;

  /* Debug */
  //metrics_n3_display(n3);
  track_n3_add_indicator_n3(t, &n3->indicator_n3);
  return 1;
  
 err:
  metrics_n3_free(n3);
  return 0;
}

static void metrics_reset(struct indicator *i)
{
}

int metrics_init(struct metrics *ctx, unique_id_t uid)
{
  return indicator_init(&ctx->indicator, uid, metrics_feed, metrics_reset);
}

void metrics_release(struct metrics *ctx)
{
  indicator_release(&ctx->indicator);
}
