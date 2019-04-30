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
#include "framework/slist.h"
#include "framework/timeline.h"

#define __indicator__(x) ((struct indicator*)(x))

/* Heritable */
#define __indicator_init__(ctx, id, feed, reset)        \
  indicator_init(__indicator__(ctx), id, feed, reset)
#define __indicator_release__(ctx)              \
  indicator_release(__indicator__(ctx))

/* Set methods */
#define __indicator_set_string__(ctx, fmt, ...)                         \
  snprintf(__indicator__(ctx)->str, INDICATOR_STR_MAX,                  \
	   fmt, ##__VA_ARGS__)
#define __indicator_set_event__(ctx, candle, event)		\
  indicator_set_event(__indicator__(ctx), candle, event)
#define __indicator_feed__(ctx, n3)          \
  indicator_feed(__indicator__(ctx), n3);

/* Internal values get */
#define __indicator_string__(ctx) __indicator__(ctx)->str
//#define __indicator_candle_value__(self) __indicator__(self)->value

/* Define types */
struct indicator; /* FIXME : find something more elegant */
typedef int (*indicator_feed_ptr)(struct indicator*, struct timeline_track_n3*);
typedef void (*indicator_reset_ptr)(struct indicator*);

struct indicator {
  /* Inherits from slist_by_uid */
  __inherits_from__(struct slist_by_uid);
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

int indicator_feed(struct indicator *ctx, struct timeline_track_n3 *e);
void indicator_reset(struct indicator *ctx);

#define indicator_uid(ctx)			\
  __slist_by_uid__(ctx)->uid

/*
 * Indicator n3 object
 */
#define __indicator_n3__(x) ((struct indicator_n3*)(x))

/* Heritable */
#define __indicator_n3_init__(ctx, parent)		\
  indicator_n3_init(__indicator_n3__(ctx), parent)
#define __indicator_n3_release__(ctx)                \
  indicator_n3_release(__indicator_n3__(ctx))

struct indicator_n3 {
  __inherits_from__(struct slist);
  struct indicator *indicator; /* parent */
  /* TODO: Might be a good idea to add a timeline_track_entry reference here */
};

static inline int indicator_n3_init(struct indicator_n3 *ctx,
                                    struct indicator *parent)
{
  __slist_init__(ctx);
  ctx->indicator = parent;
  return 0; /* alloc rulz */
}

static inline void indicator_n3_release(struct indicator_n3 *ctx)
{
  __slist_release__(ctx);
}

#define indicator_n3_indicator_uid(ctx)		\
  indicator_uid(ctx->indicator)

#endif /* defined(INDICATOR_H) */
