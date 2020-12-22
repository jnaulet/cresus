/*
 * Cresus CommonTL - engine_v2.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/05/2019
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ENGINE_V2_H
#define ENGINE_V2_H

#include "framework/time.h"
#include "engine/portfolio.h"
#include "framework/timeline_v2.h"

/* 
 * Engine v2 orders management
 */

typedef enum { BUY, SELL } engine_v2_order_t;
typedef enum { CASH, SHARES } engine_v2_order_by_t;

struct engine_v2_order {
  /* Common items */
  unique_id_t track_uid;
  engine_v2_order_t type;
  engine_v2_order_by_t by;
  /* Content */
  double value;
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
  /* common */
  ctx->track_uid = track_uid;
  ctx->type = type;
  ctx->by = by;
  /* Content */
  ctx->value = value;
  /* Info */
  ctx->level = 0;
  return 0;
}

#define engine_v2_order_alloc(ctx, track_uid, type, value, by)   \
  DEFINE_ALLOC(struct engine_v2_order, ctx,                      \
               engine_v2_order_init, track_uid, type, value, by)
#define engine_v2_order_free(ctx)               \
  DEFINE_FREE(ctx, release_dummy)

/* Set extra infos */

static inline void
engine_v2_order_set_level(struct engine_v2_order *ctx, int level)
{
  ctx->level = level;
}

/* Shortcuts */
#define engine_v2_order_shares(ctx, quotes)     \
  ((ctx)->value / (quotes))

/*
 * Main object
 */

struct engine_v2 {
  /* Main part, the timeline */
  struct timeline_v2 *timeline_v2;
  /* Orders & more */
  plist_head_t(struct engine_v2_order) plist_orders;
  /* Positions filter */
  time_t start_time;
  time_t end_time;
  /* Stats */
  double spent; /* Total money spent */
  double earned; /* Total money earned */
  double fees; /* Total fees paid */
  struct slice *last_slice;
  /* Portfolio */
  struct portfolio portfolio;
  /* CSV graph output */
  int csv_output;
};

/* External pointer to plugin */
typedef void (*engine_v2_feed_slice_ptr)(struct engine_v2*, struct slice*);
typedef void (*engine_v2_feed_track_n3_ptr)(struct engine_v2*, struct slice*, struct track_n3*);
typedef void (*engine_v2_feed_indicator_n3_ptr)(struct engine_v2*, struct track_n3*, struct indicator_n3*);
typedef void (*engine_v2_post_slice_ptr)(struct engine_v2*, struct slice*);

struct engine_v2_interface {
  engine_v2_feed_slice_ptr feed_slice; /* On every new slice */
  engine_v2_feed_track_n3_ptr feed_track_n3; /* On every track_n3 from current slice */
  engine_v2_feed_indicator_n3_ptr feed_indicator_n3; /* On every indicator_n3 from current track_n3 */
  engine_v2_post_slice_ptr post_slice; /* After every slice */
};

int engine_v2_init(struct engine_v2 *ctx, struct timeline_v2 *t);
int engine_v2_init_ex(struct engine_v2 *ctx, struct timeline_v2 *t, int argc, char **argv);
void engine_v2_release(struct engine_v2 *ctx);

void engine_v2_run(struct engine_v2 *ctx, struct engine_v2_interface *i);
int engine_v2_set_order(struct engine_v2 *ctx, struct engine_v2_order *order);

void engine_v2_display_stats(struct engine_v2 *ctx);
/* output format is "act tuple close amount" */
void engine_v2_display_pending_orders(struct engine_v2 *ctx);

#define engine_v2_init_ex_args                          \
  "[--csv output.csv] [--start YYYY-MM-DD] "            \
  "[--stop/--end YYYY-MM-DD] [--verbose/--debug]"

#endif
