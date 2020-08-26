/*
 * Cresus v2 - metrics.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/17/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "metrics.h"

//#define DISPLAY(x) PR_INFO(#x " = %.2lf\n", x)
#define DISPLAY(x)

static void metrics_feed_anyway(struct metrics_n3 *ctx,
				struct balance_sheet_n3 *b,
				double net_income, double ebit,
				double ocf)
{
  double inventory = data(b, inventory);
  double total_liab = data(b, total_liab);
  double total_assets = data(b, total_assets);
  double total_current_assets = data(b, total_current_assets);
  double total_stockholder_equity = data(b, total_stockholder_equity);
  double total_current_liabilities = data(b, total_current_liabilities);

  double short_term_debt = data(b, short_term_debt);
  double long_term_debt = data(b, long_term_debt);

  //PR_WARN("%s\n", time_to_iso8601(b->time));
  DISPLAY(inventory);
  DISPLAY(total_liab);
  DISPLAY(total_assets);
  DISPLAY(total_current_assets);
  DISPLAY(total_stockholder_equity);
  DISPLAY(total_current_liabilities);
  DISPLAY(short_term_debt);
  DISPLAY(long_term_debt);

  /*
  PR_DBG("Last %s\n", time_to_iso8601(b->time));
  DISPLAY(b->inventory);
  DISPLAY(b->total_liab);
  DISPLAY(b->total_assets);
  DISPLAY(b->total_current_assets);
  DISPLAY(b->total_stockholder_equity);
  DISPLAY(b->total_current_liabilities);
  DISPLAY(b->short_term_debt);
  DISPLAY(b->long_term_debt);
  */
  
  ctx->pe = ctx->market_cap / net_income;
  ctx->pbv = ctx->market_cap / ctx->book_value;
  ctx->eps = net_income / ctx->shares;
  ctx->roe = net_income / total_stockholder_equity;
  ctx->roce = ebit / (total_assets - total_current_liabilities);
  ctx->bvps = ctx->book_value / ctx->shares;
  ctx->cr = total_current_assets / total_current_liabilities;
  ctx->qr = (total_current_assets - inventory) / total_current_liabilities;
  ctx->de = total_liab / total_stockholder_equity;
  ctx->da = (short_term_debt + long_term_debt) / total_assets;
  ctx->cd = ocf / (short_term_debt + long_term_debt);
}

static void metrics_feed_yearly(struct metrics_n3 *ctx,
                                struct balance_sheet_n3 *b,
                                struct income_statement_n3 *i,
				struct cash_flow_n3 *c)
{
  PR_DBG("Yearly\n");
  metrics_feed_anyway(ctx, b, i->net_income, i->ebit,
		      c->total_cash_from_operating_activities);
}

static int metrics_feed_quarterly(struct metrics_n3 *ctx,
				  struct balance_sheet_n3 *b,
				  struct income_statement_n3 *i,
				  struct cash_flow_n3 *c)
{
  double ocf = 0.0;
  double ebit = 0.0;
  double net_income = 0.0;
  
  /* Last 4 quarters */
  for(int j = 4; j--;){
    ebit += i->ebit;
    net_income += i->net_income;
    if(c) ocf += c->total_cash_from_operating_activities;
    if(!(i = (void*)list_prev_null(&i->list)))
      /* Fail-safe incomplete year */
      return -1;
  }
  
  PR_DBG("Quarterly\n");
  metrics_feed_anyway(ctx, b, net_income, ebit, ocf);
  return 0;
}

static int metrics_feed(struct indicator *i, struct track_n3 *t)
{
  struct metrics_n3 *n3;
  struct metrics *ctx = (void*)i;

  struct quotes_n3 *q = t->quotes;
  struct cash_flow_n3 *c = NULL;
  struct balance_sheet_n3 *b = NULL;
  struct income_statement_n3 *s = NULL;
  
  /* Sort by most recent data */
  if(!metrics_n3_alloc(n3, i))
    return 0;
  
  /*
   * Balance sheet
   */

  if(!(b = track_n3_get_fundamental(t, balance_sheet)))
    goto err;
  
  PR_DBG("Balance sheet %s ", time_to_iso8601(b->time));
  PR_DBG("For track n3 %s\n", time_to_iso8601(t->time));
  
  /* Basic metrics */
  n3->shares = data(b, common_stock_shares_outstanding);
  n3->market_cap = n3->shares * q->close;
  n3->working_cap = (data(b, total_current_assets) -
                     data(b, total_current_liabilities));
  n3->book_value = (data(b, total_assets) -
                    data(b, intangible_assets) -
                    data(b, total_liab));

  if(n3->shares == 0.0)
    /* Something's wrong with the data */
    goto err;
  
  /*
  DISPLAY(n3->shares);
  DISPLAY(n3->market_cap);
  DISPLAY(n3->book_value);
  */

  /*
   * Cash flow
   */
  c = t->cash_flow.yearly; /* FIXME */
  
  /*
   * Income
   */

  if(!(s = t->income_statement.quarterly))
    goto yearly;
  
  if(metrics_feed_quarterly(n3, b, s, c) != -1)
    goto out;
  
 yearly:
  if(!(s = t->income_statement.yearly))
    goto err;
  
  metrics_feed_yearly(n3, b, s, c);
  
 out:
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
