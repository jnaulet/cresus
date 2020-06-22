/*
 * Cresus CommonTL v2 - timeline_v2.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/17/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef TIMELINE_V2_H
#define TIMELINE_V2_H

#include <string.h>

#include "framework/alloc.h"
#include "framework/plist.h"
#include "framework/track.h"

/* 
 * Data structure
 *
 * Slice        |  s1  |  s2  |  s3  |  s4  |  s5  |  s6  |  s7  |  s8  | ...
 *              -      |      |      |      |      |      |      |      |
 * Track #n     | [d1] - [d2] - [d3] - [d4] - [d5] - [d6] - [d7] - [d8] - ...
 *              | [ bsq1                  ] - [ bsq2                  ] - ...
 *              | [ bsa1                                              ] - ...
 *              -
 */

/*
 * Timeline slice object
 */

struct slice {
  time64_t time;
  plist_head_t(struct track_n3) plist_track_n3s;
};

#define slice_alloc(ctx, time)                          \
  DEFINE_ALLOC(struct slice, ctx, slice_init, time)
#define slice_free(ctx)                         \
  DEFINE_FREE(ctx, slice_release)

int slice_init(struct slice *ctx, time64_t time);
void slice_release(struct slice *ctx);

struct track_n3 *slice_get_track_n3(struct slice *ctx, unique_id_t uid);

#define slice_for_each_track_n3(ctx, n3)                                \
  for(struct plist *__p__ = ((ctx)->plist_track_n3s.next);		\
      __p__ != &(ctx)->plist_track_n3s &&				\
	(n3 = ((struct track_n3*)(__p__)->ptr));                        \
      __p__ = __p__->next)

/*
 * Timeline
 */

struct timeline {
  plist_head_t(struct slice) by_slice;
  plist_head_t(struct track) by_track;
};

int timeline_init(struct timeline *ctx);
void timeline_release(struct timeline *ctx);

int timeline_add_track(struct timeline *ctx, struct track *track);
struct track *timeline_find_track_by_uid(struct timeline *ctx, unique_id_t uid);
/* Backwards compatibility */
#define timeline_find_track(ctx, uid) timeline_find_track_by_uid(ctx, uid)

#endif
