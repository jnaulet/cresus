/*
 * Cresus CommonTL - engine_v2.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/05/2019
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ENGINE_V2_H
#define ENGINE_V2_H

#include "framework/time64.h"
#include "framework/timeline.h"

#include "engine/portfolio.h"
#include "engine/common_opt.h"

/* 
 * Engine v2 orders management
 */

typedef enum { BUY, SELL } engine_v2_order_t;
typedef enum { CASH, SHARES } engine_v2_order_by_t;

struct engine_v2_order {
  __inherits_from__(struct list);
  /* Common items */
  unique_id_t track_uid;
  engine_v2_order_t type;
  engine_v2_order_by_t by;
  /* Content */
  double value;
  /* Leverage */
  double funding, ratio, stoploss;
  /* For info */
  int level; 
};

static inline int
engine_v2_order_init(struct engine_v2_order *ctx,
		     unique_id_t track_uid,
		     engine_v2_order_t type,
		     double value,
		     engine_v2_order_by_t by)
{
  __list_init__(ctx); /* super() */
  /* common */
  ctx->track_uid = track_uid;
  ctx->type = type;
  ctx->by = by;
  /* Content */
  ctx->value = value;
  /* Leverage */
  ctx->funding = 0.0;
  ctx->ratio = 1.0;
  ctx->stoploss = 0.0;
  /* Info */
  ctx->level = 0;
  return 0;
}

#define engine_v2_order_alloc(ctx, track_uid, type, value, by)   \
  DEFINE_ALLOC(struct engine_v2_order, ctx,			 \
	       engine_v2_order_init, track_uid, type, value, by)
#define engine_v2_order_free(ctx)		\
  DEFINE_FREE(ctx, release_dummy)

/* Set extra infos */

static inline void
engine_v2_order_set_level(struct engine_v2_order *ctx, int level)
{
  ctx->level = level;
}

static inline void
engine_v2_order_set_leverage(struct engine_v2_order *ctx,
			     double funding, double ratio,
			     double stoploss)
{
  ctx->funding = funding;
  ctx->stoploss = funding;
  ctx->ratio = ratio;
}

/* Shortcuts */
#define engine_v2_order_set_turbo(ctx, funding, ratio)		\
  engine_v2_order_set_leverage(ctx, funding, ratio, funding)
#define engine_v2_order_shares(ctx, price)	\
  ((ctx)->value / ((price) - (ctx)->funding))

/*
 * Main object
 */

struct engine_v2 {
  struct timeline *timeline;
  /* Orders & more */
  list_head_t(struct engine_v2_order) list_orders;
  /* Positions filter */
  time64_t start_time;
  time64_t end_time;
  /* Stats */
  double spent; /* Total money spent */
  double earned; /* Total money earned */
  double fees; /* Total fees paid */
  struct timeline_slice *last_slice;
  /* Portfolio */
  struct portfolio portfolio;
  /* CSV graph output */
  int csv_output;
};

/* External pointer to plugin */
typedef void (*engine_v2_feed_slice_ptr)(struct engine_v2*, struct timeline_slice*);
typedef void (*engine_v2_feed_track_n3_ptr)(struct engine_v2*, struct timeline_slice*, struct timeline_track_n3*);
typedef void (*engine_v2_feed_indicator_n3_ptr)(struct engine_v2*, struct timeline_track_n3*, struct indicator_n3*);
typedef void (*engine_v2_post_slice_ptr)(struct engine_v2*, struct timeline_slice*);

struct engine_v2_interface {
  engine_v2_feed_slice_ptr feed_slice; /* On every new slice */
  engine_v2_feed_track_n3_ptr feed_track_n3; /* On every track_n3 from current slice */
  engine_v2_feed_indicator_n3_ptr feed_indicator_n3; /* On every indicator_n3 from current track_n3 */
  engine_v2_post_slice_ptr post_slice; /* After every slice */
};

int engine_v2_init(struct engine_v2 *ctx, struct timeline *t);
void engine_v2_release(struct engine_v2 *ctx);

int engine_v2_set_common_opt(struct engine_v2 *ctx, struct common_opt *opt);
void engine_v2_run(struct engine_v2 *ctx, struct engine_v2_interface *i);
int engine_v2_set_order(struct engine_v2 *ctx, struct engine_v2_order *order);

#endif
