/*
 * Cresus Balance Sheet - balance-sheet.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/11/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef BALANCE_SHEET_H
#define BALANCE_SHEET_H

#include <string.h>
#include <limits.h>

#include "framework/time.h"
#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/types.h"

/* Generic object */
struct balance_sheet_n3 {
  /* Inherits from list */
  struct list list;
  /* Time */
  time_t time;
  /* Data */
  double total_assets;
  double intangible_assets;
  double earning_assets;
  double other_current_assets;
  double total_liab;
  double total_stockholder_equity;
  double deferred_long_term_liab;
  double other_current_liab;
  double common_stock;
  double retained_earnings;
  double other_liab;
  double good_will;
  double other_assets;
  double cash;
  double total_current_liabilities;
  double short_term_debt;
  double short_long_term_debt;
  double short_long_term_debt_total;
  double other_stockholder_equity;
  double property_plant_equipment;
  double total_current_assets;
  double long_term_investments;
  double net_tangible_assets;
  double short_term_investments;
  double net_receivables;
  double long_term_debt;
  double inventory;
  double accounts_payable;
  double total_permanent_equity;
  double noncontrolling_interest_in_consolidated_entity;
  double temporary_equity_redeemable_noncontrolling_interests;
  double accumulated_other_comprehensive_income;
  double additional_paid_in_capital;
  double common_stock_total_equity;
  double preferred_stock_total_equity;
  double retained_earnings_total_equity;
  double treasury_stock;
  double accumulated_amortization;
  double non_currrent_assets_other;
  double deferred_long_term_asset_charges;
  double non_current_assets_total;
  double capital_lease_obligations;
  double long_term_debt_total;
  double non_current_liabilities_other;
  double non_current_liabilities_total;
  double negative_goodwill;
  double warrants;
  double preferred_stock_redeemable;
  double capital_surpluse;
  double liabilities_and_stockholders_equity;
  double cash_and_short_term_investments;
  double property_plant_and_equipment_gross;
  double accumulated_depreciation;
  double common_stock_shares_outstanding;
};

static inline int balance_sheet_n3_init(struct balance_sheet_n3 *ctx,
                                        time_t time)
{
  list_init(&ctx->list); /* super() */
  ctx->time = time;
  return 0;
}

static inline void balance_sheet_n3_release(struct balance_sheet_n3 *ctx)
{
  list_release(&ctx->list);
}

#define balance_sheet_n3_alloc(ctx, time)				\
  DEFINE_ALLOC(struct balance_sheet_n3, ctx, balance_sheet_n3_init, time)
#define balance_sheet_n3_free(ctx)              \
  DEFINE_FREE(ctx, balance_sheet_n3_release)

/* typedefs */
struct balance_sheet;

struct balance_sheet_ops {
  int (*init)(struct balance_sheet *ctx);
  void (*release)(struct balance_sheet *ctx);
  struct balance_sheet_n3 *(*read)(struct balance_sheet *ctx);
};

struct balance_sheet {
  list_head_t(struct balance_sheet_n3) list_balance_sheet_n3s;
  char filename[256], ext[256];
  struct balance_sheet_ops *ops;
  period_t period;
  void *private;
};

int balance_sheet_init(struct balance_sheet *ctx, const char *filename, period_t period);
void balance_sheet_release(struct balance_sheet *ctx);

#define balance_sheet_alloc(ctx, filename, period)                      \
  DEFINE_ALLOC(struct balance_sheet, ctx, balance_sheet_init, filename, period)
#define balance_sheet_free(ctx)                 \
  DEFINE_FREE(ctx, balance_sheet_release)

#endif
