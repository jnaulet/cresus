/*
 * Cresus EVO - engine.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/21/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__engine__
#define __Cresus_EVO__engine__

#include "engine/candle.h"
#include "engine/position.h"
#include "engine/timeline.h"
#include "framework/common_opt.h"

struct engine {
  struct timeline *timeline;
  /* Portfolio */
  list_head_t(struct position) list_position;
  /* Money management */
  double npos_buy, npos_sell;
  double amount; /* FIXME: find another name */
  double earnings; /* Find another name */
  double fees; /* Total fees */
  double balance; /* Buy/sell balance */
  /* Statistics */
  int nbuy, nsell; /* As indicated */
  double max_drawdown; /* Max money invested at the same time */
  double transaction_fee; /* Cost per transaction */
  /* Last close value */
  double close;
  /* Positions filter */
  time_info_t start_time;
  time_info_t end_time;
  /* Display info */
  int quiet;
  /* CSV graph output */
  int csv_output;
  double csv_ref;
};

#define engine_set_transaction_fee(ctx, fee)	\
  (ctx)->transaction_fee = fee
#define engine_set_start_time(ctx, time_info)	\
  (ctx)->start_time = time_info
#define engine_set_end_time(ctx, time_info)	\
  (ctx)->end_time = time_info
#define engine_set_quiet(ctx, quiet)		\
  (ctx)->quiet = quiet
#define engine_set_csv_output(ctx, boolean)     \
  (ctx)->csv_output = boolean

/* External pointer to plugin */
typedef int (*engine_feed_ptr)(struct engine*, struct timeline*, struct timeline_entry*);

int engine_init(struct engine *e, struct timeline *t);
void engine_release(struct engine *e);

int engine_set_common_opt(struct engine *e, struct common_opt *opt);

void engine_run(struct engine *e, engine_feed_ptr feed);

int engine_set_order(struct engine *e, position_t type,
		     double value, position_req_t req, 
		     struct cert *cert);

/* For backwards compatibility */
int engine_place_order(struct engine *ctx, position_t type,
		       position_req_t req, double value) __attribute__((deprecated));

double engine_npos(struct engine *ctx, int *nrec);
double engine_assets_value(struct engine *ctx, double close);
double engine_assets_original_value(struct engine *ctx);

struct engine_stats {
  double amount;
  double earnings;
  double fees;
  double assets_value;
  double total_value;
  double roi;
  double balance;
  double max_drawdown;
  double rrr;
};

void engine_get_stats(struct engine *ctx, struct engine_stats *stats);
void engine_display_stats_r(struct engine *ctx, struct engine_stats *stats);
void engine_display_stats(struct engine *ctx);

#include <math.h>

#define engine_stats_init(stats) memset((stats), 0, sizeof(*stats));
static inline void engine_stats_aggregate(struct engine_stats *dst,
					  struct engine_stats *src)
{
  /* Accumulate stats */
  dst->amount += src->amount;
  dst->earnings += src->earnings;
  dst->fees += src->fees;
  dst->assets_value += src->assets_value;
  dst->total_value += src->total_value;
  dst->roi = ((dst->total_value / dst->amount) - 1.0) * 100.0;
  dst->balance += src->balance;
  dst->max_drawdown += src->max_drawdown;
  dst->rrr = dst->balance / fabs(dst->max_drawdown) + 1.0;
}

#endif /* defined(__Cresus_EVO__engine__) */
