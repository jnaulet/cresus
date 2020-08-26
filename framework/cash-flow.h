/*
 * Cresus Income Statement - income-statement.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/15/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef CASH_FLOW_H
#define CASH_FLOW_H

#include <string.h>
#include <limits.h>

#include "framework/list.h"
#include "framework/time.h"
#include "framework/alloc.h"
#include "framework/types.h"

/* Generic object */
struct cash_flow_n3 {
  /* Inherits from list */
  struct list list;
  /* Time */
  time_t time;
  /* Data */
  double investments;
  double change_to_liabilities;
  double total_cashflows_from_investing_activities;
  double net_borrowings;
  double total_cash_from_financing_activities;
  double change_to_operating_activities;
  double net_income;
  double change_in_cash;
  double total_cash_from_operating_activities;
  double depreciation;
  double other_cashflows_from_investing_activities;
  double dividends_paid;
  double change_to_inventory;
  double change_to_account_receivables;
  double sale_purchase_of_stock;
  double other_cashflows_from_financing_activities;
  double change_to_netincome;
  double capital_expenditures;
  double change_receivables;
  double cash_flows_other_operating;
  double exchange_rate_changes;
  double cash_and_cash_equivalents_changes;
};

static inline int cash_flow_n3_init(struct cash_flow_n3 *ctx,
                                    time_t time)
{
  list_init(&ctx->list); /* super() */
  ctx->time = time;
  return 0;
}

static inline void cash_flow_n3_release(struct cash_flow_n3 *ctx)
{
  list_release(&ctx->list);
}

#define cash_flow_n3_alloc(ctx, time)                                   \
  DEFINE_ALLOC(struct cash_flow_n3, ctx, cash_flow_n3_init, time)
#define cash_flow_n3_free(ctx)                  \
  DEFINE_FREE(ctx, cash_flow_n3_release)

/* typedefs */
struct cash_flow;

struct cash_flow_ops {
  int (*init)(struct cash_flow *ctx);
  void (*release)(struct cash_flow *ctx);
  struct cash_flow_n3 *(*read)(struct cash_flow *ctx);
};

struct cash_flow {
  list_head_t(struct cash_flow_n3) list_cash_flow_n3s;
  period_t period;
  char filename[256];
  struct cash_flow_ops *ops;
  void *private;
};

int cash_flow_init(struct cash_flow *ctx, const char *filename, period_t period);
void cash_flow_release(struct cash_flow *ctx);

#define cash_flow_alloc(ctx, filename, period)                          \
  DEFINE_ALLOC(struct cash_flow, ctx, cash_flow_init, filename, period)
#define cash_flow_free(ctx)                     \
  DEFINE_FREE(ctx, cash_flow_release)

#endif
