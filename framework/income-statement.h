/*
 * Cresus Income Statement - income-statement.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/15/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef INCOME_STATEMENT_H
#define INCOME_STATEMENT_H

#include <string.h>
#include <limits.h>

#include "framework/list.h"
#include "framework/time.h"
#include "framework/alloc.h"
#include "framework/types.h"

/* Generic object */
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

/* typedefs */
struct income_statement;

struct income_statement_ops {
  int (*init)(struct income_statement *ctx);
  void (*release)(struct income_statement *ctx);
  struct income_statement_n3 *(*read)(struct income_statement *ctx);
};

struct income_statement {
  list_head_t(struct income_statement_n3) list_income_statement_n3s;
  period_t period;
  char filename[256];
  struct income_statement_ops *ops;
  void *private;
};

int income_statement_init(struct income_statement *ctx, const char *filename, period_t period);
void income_statement_release(struct income_statement *ctx);

#define income_statement_alloc(ctx, filename, ext)                      \
  DEFINE_ALLOC(struct income_statement, ctx, income_statement_init, filename, ext)
#define income_statement_free(ctx)              \
  DEFINE_FREE(ctx, income_statement_release)

#endif
