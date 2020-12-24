/*
 * Cresus TypesExperiment - track.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 07/27/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef TRACK_H
#define TRACK_H

#include "framework/list.h"
#include "framework/time.h"
#include "framework/alloc.h"
#include "framework/plist.h"

#include "framework/quotes.h"
#include "framework/balance-sheet.h"
#include "framework/income-statement.h"
#include "framework/cash-flow.h"

#include "framework/splits.h"
#include "framework/dividends.h"

struct slice;
struct indicator;
struct indicator_n3;

struct track_n3 {
  /* Internal entries */
  time_t time;
  struct quotes_n3 *quotes;
  
  /* Balance sheet */
  union {
    struct {
      struct balance_sheet_n3 *quarterly;
      struct balance_sheet_n3 *yearly;
    };
    struct balance_sheet_n3 *period[PERIOD_MAX];
  } balance_sheet;

  /* Income statement */
  union {
    struct {
      struct income_statement_n3 *quarterly;
      struct income_statement_n3 *yearly;
    };
    struct income_statement_n3 *period[PERIOD_MAX];
  } income_statement;

  /* Cash flow */
  union {
    struct {
      struct cash_flow_n3 *quarterly;
      struct cash_flow_n3 *yearly;
    };
    struct cash_flow_n3 *period[PERIOD_MAX];
  } cash_flow;

  /* Dividends */
  struct dividends_n3 *dividends;

  /* Splits */
  struct splits_n3 *splits;
  
  /* Indicators */
  plist_head_t(struct indicator_n3) plist_indicator_n3s;
  /* Where are we from ? */
  struct track *track;
  struct slice *slice;
  /* Parent */
  struct plist *plist;
};

/* Shortcuts */

#define period_of(type, x)						\
  ((void*)(x) == (void*)((type)->quarterly) ? QUARTERLY : YEARLY)

/* Init */

#define track_n3_alloc(ctx, quotes_n3, track)                            \
  DEFINE_ALLOC(struct track_n3, ctx, track_n3_init, quotes_n3, track)
#define track_n3_free(ctx)                      \
  DEFINE_FREE(ctx, track_n3_release)

int track_n3_init(struct track_n3 *ctx, struct quotes_n3 *quotes_n3, struct track *track);
void track_n3_release(struct track_n3 *ctx);

/* Methods */

#define track_n3_get_fundamental(ctx, type)		\
  ((ctx)->type.quarterly != NULL ?			\
   (ctx)->type.quarterly :				\
   (ctx)->type.yearly)

#define track_n3_add_indicator_n3(ctx, indicator_n3)            \
  plist_add_ptr(&(ctx)->plist_indicator_n3s, indicator_n3)

struct track_n3 *
track_n3_prev(struct track_n3 *ctx, int n);
struct indicator_n3 *
track_n3_get_indicator_n3(struct track_n3 *ctx,
                          unique_id_t indicator_uid);

#define track_n3_for_each_indicator_n3(ctx, n3)                 \
  for(struct plist *__p__ = (ctx)->plist_indicator_n3s.next;    \
      __p__ != &(ctx)->plist_indicator_n3s &&                   \
        (n3 = (struct indicator_n3*)(__p__)->ptr);              \
      __p__ = __p__->next)

const char *track_n3_str(struct track_n3 *ctx);
const char *track_n3_str_r(struct track_n3 *ctx, char *buf);

/*
 * Timeline track object
 */

struct track {
  unique_id_t uid;
#define TRACK_NAME_MAX 64
  char name[TRACK_NAME_MAX];
  /* Here's the beginning of the track */
  plist_head_t(struct track_n3) plist_track_n3s;
  /* The indicators we want to play on that particular track */
  plist_head_t(struct indicator) plist_indicators;
  /* Fee depends on track, not engine */
  double amount;
  double transaction_fee;
  /* Mioght be useful */
  size_t length;
  /* User might want ot expand this object */
  void *private;
};

#define track_alloc(ctx, uid, name, quotes)                             \
  DEFINE_ALLOC(struct track, ctx, track_init, uid, name, quotes)
#define track_free(ctx)                         \
  DEFINE_FREE(ctx, track_release)

int track_init(struct track *ctx, unique_id_t uid, const char *name, struct quotes *quotes);
void track_release(struct track *ctx);

/*
 * Methods
 */

int track_add_balance_sheet(struct track *ctx, struct balance_sheet *b);
int track_add_income_statement(struct track *ctx, struct income_statement *i);
int track_add_cash_flow(struct track *ctx, struct cash_flow *c);
int track_add_dividends(struct track *ctx, struct dividends *d);
int track_add_splits(struct track *ctx, struct splits *s);

#define track_add_indicator(ctx, indicator)             \
  plist_add_ptr(&(ctx)->plist_indicators, indicator)
#define track_set_fee(ctx, fee)                 \
  (ctx)->transaction_fee = fee;
#define track_get_amount(ctx, value)			\
  ((ctx)->amount != 0 ? (ctx)->amount : (value))

#endif
