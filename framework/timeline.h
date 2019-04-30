/*
 * Cresus CommonTL - timeline.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/05/2019
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef TIMELINE_H
#define TIMELINE_H

#include <string.h>

#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/slist.h"
#include "framework/input.h"

/*
 * Order of operation :
 * 1) Read input
 * 2) Create slice if necessary. Sort it
 * 3) Create track n3. Append to previous. Register slice
 * 4) Create slice n3. Register track n3. Append to slice
 * 5) Back to 1
 * 6) Play indicators once slices are sync
 */

/* Required for compilation */
struct indicator;
struct indicator_n3;
struct timeline_track;
struct timeline_slice;

/*
 * Data hierarchy
 *
 * Timeline by slice
 * |- slice #0
 *    |- slice_n3 -> slice_n3 -> slice_n3
 * |- slice #x
 *    |- slice_n3 -> slice_n3 -> slice_n3
 * |- ...
 *
 * Timeline by track
 * |- track #0
 *    |- track_n3 <> track_n3 <> track_n3 <> ...
 * |- track #x
 *    |- track_n3 <> track_n3 <> track_n3 <> ...
 * |- ...
 *
 */

/*
 * Linear access
 */
struct timeline_track_n3 {
  /* Can be parsed either way */
  __inherits_from__(struct list);
  /* Internal data */
  __implements__(input_n3_interface);
  /* Where are we from ? */
  struct timeline_track *track; /* Who created this */
  struct timeline_slice *slice; /* By-slice sorting */
  /* More data, append later */
  slist_head_t(struct indicator_n3) slist_indicator_n3s;
};

static inline int
timeline_track_n3_init(struct timeline_track_n3 *ctx,
                       struct input_n3 *input_n3,
                       struct timeline_track *track,
                       struct timeline_slice *slice)
{
  __list_init__(ctx); /* super() */
  input_n3_interface_copy(ctx, input_n3);
  ctx->track = track;
  ctx->slice = slice;
  slist_head_init(&ctx->slist_indicator_n3s);
  return 0;
}

#define timeline_track_n3_alloc(ctx, input_n3, track, slice)            \
  DEFINE_ALLOC(struct timeline_track_n3, ctx, timeline_track_n3_init,   \
               input_n3, track, slice)

/* Methods */
static inline void
timeline_track_n3_add_indicator_n3(struct timeline_track_n3 *ctx,
                                   struct indicator_n3 *indicator_n3)
{
  __slist_push__(&ctx->slist_indicator_n3s, indicator_n3);
}

static inline struct indicator_n3 *
timeline_track_n3_get_indicator_n3(struct timeline_track_n3 *ctx,
                                   unique_id_t indicator_uid)
{
  return (struct indicator_n3*)
    __slist_by_uid_find__(&ctx->slist_indicator_n3s, indicator_uid);
}

#define timeline_track_n3_for_each_indicator_n3(ctx, n3)                \
  for(struct slist *__ptr__ =                                           \
        __slist__((ctx)->slist_indicator_n3s.next);                     \
      __ptr__ != NULL &&                                                \
        (n3 = (struct indicator_n3*)(__ptr__));                         \
      __ptr__ = __ptr__->next)

const char *timeline_track_n3_str(struct timeline_track_n3 *ctx);
const char *timeline_track_n3_str_r(struct timeline_track_n3 *ctx, char *buf);

#define timeline_track_n3_track_uid(ctx)	\
  timeline_track_uid((ctx)->track)

struct timeline_track {
  /* It's a slist of lists */
  __inherits_from__(struct slist_by_uid);
#define TIMELINE_TRACK_NAME_MAX 64
  char name[TIMELINE_TRACK_NAME_MAX];
  /* Here's the beginning of the track */
  list_head_t(struct timeline_track_n3) list_track_n3s;
  /* The indicators we want to play on that particular track */
  slist_head_t(struct indicator) slist_indicators;
};

static inline int
timeline_track_init(struct timeline_track *ctx, unique_id_t uid,
                    const char *name)
{
  __slist_by_uid_init__(ctx, uid); /* super() */
  strncpy(ctx->name, name, sizeof(ctx->name));
  list_head_init(&ctx->list_track_n3s);
  slist_head_init(&ctx->slist_indicators);
  return uid;
}

#define timeline_track_alloc(ctx, uid, name)                            \
  DEFINE_ALLOC(struct timeline_track, ctx, timeline_track_init, uid, name)

static inline void
timeline_track_add_indicator(struct timeline_track *ctx,
			     struct indicator *indicator)
{
  __slist_push__(&ctx->slist_indicators, indicator);
}

#define timeline_track_uid(ctx)			\
  __slist_by_uid__(ctx)->uid

/*
 * Access by slice / indice / time
 */

/* /!\ slice_n3 should never be accessed directly by user /!\ */
struct timeline_slice_n3 {
  __inherits_from__(struct slist);
  struct timeline_track_n3 *track_n3;
};

struct timeline_slice {
  __inherits_from__(struct list);
  /* It's a time slice */
  time64_t time; /* No granularity required ? */
  /* Containing one or more timeline_slices_n3s */
  slist_head_t(struct timeline_slice_n3) slist_slice_n3s;
};

static inline int
timeline_slice_init(struct timeline_slice *ctx, time64_t time)
{
  __list_init__(ctx); /* super() */
  ctx->time = time;
  slist_head_init(&ctx->slist_slice_n3s);
  return 0;
}

#define timeline_slice_alloc(ctx, time)                                 \
  DEFINE_ALLOC(struct timeline_slice, ctx, timeline_slice_init, time)

struct timeline_track_n3*
timeline_slice_get_track_n3(struct timeline_slice *ctx, unique_id_t uid);

#define timeline_slice_for_each_track_n3(ctx, n3)                       \
  for(struct slist *__ptr__ = __slist__((ctx)->slist_slice_n3s.next);   \
      __ptr__ != NULL &&                                                \
        (n3 = ((struct timeline_slice_n3*)(__ptr__))->track_n3);        \
      __ptr__ = __ptr__->next)

/*
 * The final timeline object
 */

struct timeline {
  list_head_t(struct timeline_slice) by_slice;
  slist_head_t(struct timeline_track) by_track;
};

/* Interfaces */
int timeline_init(struct timeline *ctx);
void timeline_release(struct timeline *ctx);

int timeline_add_track(struct timeline *ctx,
		       struct timeline_track *track,
		       struct input *input);

/* Run & sync all */
int timeline_run_and_sync(struct timeline *ctx);

#endif
