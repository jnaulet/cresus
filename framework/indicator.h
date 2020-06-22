/*
 * Cresus EVO - indicator.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/21/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef INDICATOR_H
#define INDICATOR_H

#include "framework/types.h"
#include "framework/track.h"

/* Define types */
struct indicator;
typedef int (*indicator_feed_ptr)(struct indicator*, struct track_n3*);
typedef void (*indicator_reset_ptr)(struct indicator*);

struct indicator {
  /* Unique ID */
  unique_id_t uid;
  /* Fn pointers */
  indicator_feed_ptr feed;
  indicator_reset_ptr reset;
  /* Name */
#define INDICATOR_STR_MAX 64
  char str[INDICATOR_STR_MAX];
  /* Status */
  int is_empty;
};

int indicator_init(struct indicator *ctx, unique_id_t uid, indicator_feed_ptr feed, indicator_reset_ptr reset);
void indicator_release(struct indicator *ctx);

int indicator_feed(struct indicator *ctx, struct track_n3 *e);
void indicator_reset(struct indicator *ctx);

/* Inherited accessors */
#define indicator_str(ctx) ((ctx)->str)
#define indicator_set_string(ctx, fmt, ...)			\
  snprintf((ctx)->str, INDICATOR_STR_MAX, fmt, ##__VA_ARGS__)

/*
 * Indicator n3 object
 */

struct indicator_n3 {
  struct indicator *indicator; /* parent */
};

static inline int indicator_n3_init(struct indicator_n3 *ctx,
                                    struct indicator *parent)
{
  ctx->indicator = parent;
  return 0; /* alloc rulz */
}

static inline void indicator_n3_release(struct indicator_n3 *ctx)
{
}

/* Accessors */
#define indicator_n3_indicator_uid(ctx)         \
  ((ctx)->indicator->uid)

#endif /* defined(INDICATOR_H) */
