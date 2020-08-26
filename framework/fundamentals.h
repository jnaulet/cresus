/*
 * Cresus - fundamentals.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/25/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef FUNDAMENTALS_H
#define FUNDAMENTALS_H

#include <time.h>
#include <string.h>
#include <limits.h>

#include "framework/list.h"
#include "framework/alloc.h"

/*
 * Balance sheet
 */

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

/*
 * Income statement
 */

struct income_statement_n3 {
  /* Inherits from list */
  struct list list;
  /* Time */
  time_t time;
  /* Internal data */
  double research_development;
  double effect_of_accounting_charges;
  double income_before_tax;
  double minority_interest;
  double net_income;
  double selling_general_administrative;
  double gross_profit;
  double ebit;
  double non_operating_income_net_other;
  double operating_income;
  double other_operating_expenses;
  double interest_expense;
  double tax_provision;
  double interest_income;
  double net_interest_income;
  double extraordinary_items;
  double non_recurring;
  double other_items;
  double income_tax_expense;
  double total_revenue;
  double total_operating_expenses;
  double cost_of_revenue;
  double total_other_income_expense_net;
  double discontinued_operations;
  double net_income_from_continuing_ops;
  double net_income_applicable_to_common_shares;
  double preferred_stock_and_other_adjustments;
};

static inline int income_statement_n3_init(struct income_statement_n3 *ctx,
                                           time_t time)
{
  list_init(&ctx->list); /* super() */
  ctx->time = time;
  return 0;
}

static inline void income_statement_n3_release(struct income_statement_n3 *ctx)
{
  list_release(&ctx->list);
}

#define income_statement_n3_alloc(ctx, time)				\
  DEFINE_ALLOC(struct income_statement_n3, ctx, income_statement_n3_init, time)
#define income_statement_n3_free(ctx)           \
  DEFINE_FREE(ctx, income_statement_n3_release)

/* 
 * Cash flow
 */

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
struct fundamentals;

struct fundamentals_ops {
  int (*init)(struct fundamentals *ctx);
  void (*release)(struct fundamentals *ctx);
  struct balance_sheet_n3 *(*read_bq)(struct fundamentals *ctx);
  struct balance_sheet_n3 *(*read_by)(struct fundamentals *ctx);
  struct income_statement_n3 *(*read_iq)(struct fundamentals *ctx);
  struct income_statement_n3 *(*read_iy)(struct fundamentals *ctx);
  struct cash_flow_n3 *(*read_cq)(struct fundamentals *ctx);
  struct cash_flow_n3 *(*read_cy)(struct fundamentals *ctx);
};

struct fundamentals {
  char filename[256], ext[256];
  struct fundamentals_ops *ops;
  void *private;
};

int fundamentals_init(struct fundamentals *ctx, const char *filename);
void fundamentals_release(struct fundamentals *ctx);

#define fundamentals_alloc(ctx, filename)                               \
  DEFINE_ALLOC(struct fundamentals, ctx, fundamentals_init, filename)
#define fundamentals_free(ctx)                  \
  DEFINE_FREE(ctx, fundamentals_release)

#endif
