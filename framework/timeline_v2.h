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
 * Timeline_V2 slice object
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
 * Timeline_V2
 */

struct timeline_v2 {
  plist_head_t(struct slice) by_slice;
  plist_head_t(struct track) by_track;
};

typedef void (*custom_opt_ptr)(struct timeline_v2*, char*, char*);
typedef void (*customize_track_ptr)(struct timeline_v2*, struct track*);

struct timeline_v2_ex_interface {
  custom_opt_ptr custom_opt; /* Our own options */
  customize_track_ptr customize_track; /* Finalize track creation */
};

int timeline_v2_init(struct timeline_v2 *ctx);
void timeline_v2_release(struct timeline_v2 *ctx);

/* Special case */
int timeline_v2_init_ex(struct timeline_v2 *ctx, int argc, char **argv,
			struct timeline_v2_ex_interface *itf);

int timeline_v2_add_track(struct timeline_v2 *ctx, struct track *track);
struct track *timeline_v2_find_track_by_uid(struct timeline_v2 *ctx, unique_id_t uid);

/* Backwards compatibility */
#define timeline_v2_find_track(ctx, uid) timeline_v2_find_track_by_uid(ctx, uid)

#define timeline_v2_ex_args					\
  "--track quotes.type [--balance-sheet balance.type] "		\
  "[--fee fee] [--amount amount]"

#endif
