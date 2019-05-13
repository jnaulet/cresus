/*
 * Cresus CommonTL - timeline.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/05/2019
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>

#include "framework/types.h"
#include "framework/verbose.h"
#include "framework/timeline.h"
#include "framework/indicator.h"

/*
 * Timeline track entry object
 */
const char *timeline_track_n3_str(struct timeline_track_n3 *ctx)
{
  static char buf[256];
  return timeline_track_n3_str_r(ctx, buf);
}

const char *timeline_track_n3_str_r(struct timeline_track_n3 *ctx,
                                    char *buf)
{
  sprintf(buf, "%s: %s " input_n3_interface_fmt,
          ctx->track->name,
          time64_str(ctx->time, GR_DAY), /* ! */
          input_n3_interface_args(ctx));
  
  return buf;
}

struct indicator_n3 *
timeline_track_n3_get_indicator_n3(struct timeline_track_n3 *ctx,
                                   unique_id_t indicator_uid)
{
  struct indicator_n3 *ptr;
  __slist_for_each__(&ctx->slist_indicator_n3s, ptr){
    if(indicator_n3_indicator_uid(ptr) == indicator_uid)
      return ptr;
  }
  
  return NULL;
}

static int timeline_track_add_track_n3(struct timeline_track *ctx,
                                       struct timeline_track_n3 *track_n3)
{
  struct timeline_track_n3 *ptr;
  
  __list_for_each_prev__(&ctx->list_track_n3s, ptr){
    /* Compare */
    time64_t c = TIME64CMP(ptr->time, track_n3->time, GR_DAY);
    /* ptr already exists */
    if(!c){
      PR_WARN("timeline.c: %s %s track_n3 already exists ! Discard...\n",
              ctx->name, time64_str(track_n3->time, GR_DAY));
      return -1;
    }
    /* ptr is ahead, sort */
    if(c < 0){
      __list_add__(ptr, track_n3);
      /* Debug */
      static char buf0[12], buf1[12];
      PR_DBG("timeline.c: %s inserted after %s\n",
             time64_str_r(track_n3->time, GR_DAY, buf0),
             time64_str_r(ptr->time, GR_DAY, buf1));
      return 0;
    }
  }
  
  /* Something's wrong or no n3s yet */
  __list_add_tail__(&ctx->list_track_n3s, track_n3);
  return -1;
}

/*
 * Timeline slice + n3 object
 */

/* Slice n3 "private" functions */
static int timeline_slice_n3_init(struct timeline_slice_n3 *ctx,
				  struct timeline_track_n3 *track_n3)
{
  __slist_init__(ctx); /* super() */
  ctx->track_n3 = track_n3;
  return 0;
}

#define timeline_slice_n3_alloc(ctx, track_n3)                          \
  DEFINE_ALLOC(struct timeline_slice_n3, ctx,				\
	       timeline_slice_n3_init, track_n3)

struct timeline_track_n3*
timeline_slice_get_track_n3(struct timeline_slice *ctx,
                            unique_id_t uid)
{
  struct timeline_slice_n3 *slice_n3;
  __slist_for_each__(&ctx->slist_slice_n3s, slice_n3){
    if(timeline_track_n3_track_uid(slice_n3->track_n3) == uid)
      return slice_n3->track_n3;
  }
  
  return NULL;
}

/*
 * Timeline object
 */
static struct timeline_slice *
timeline_get_slice_anyway(struct timeline *ctx, time64_t time)
{
  struct timeline_slice *ptr;
  
  __list_for_each__(&ctx->by_slice, ptr){
    time64_t cmp = TIME64CMP(ptr->time, time, GR_DAY);
    /* Slice already exists, we go out */
    if(!cmp){
      PR_DBG("timeline.c: slice already exists\n");
      goto out;
    }
    /* Slice is ahead, sort */
    if(cmp > 0){
      struct timeline_slice *ahead = ptr;
      __try__(!timeline_slice_alloc(ptr, time), err);
      __list_add_tail__(ahead, ptr); /* Insert before */
      /* Debug */
      static char buf0[12], buf1[12];
      PR_DBG("timeline.c: slice %s is missing, insertion before %s\n",
	     time64_str_r(time, GR_DAY, buf0),
	     time64_str_r(ahead->time, GR_DAY, buf1));
      /* Jump outside anyway */
      goto out;
    }
  }
  
  /* Slice doesn't exist, we create it */
  __try__(!timeline_slice_alloc(ptr, time), err);
  __list_add_tail__(&ctx->by_slice, ptr);
  
  /* Debug */
  PR_DBG("timeline.c: new slice at %s\n", time64_str(time, GR_DAY));
  
 out:
  return ptr;

 __catch__(err):
  PR_ERR("%s: can't allocate slice object\n", __FUNCTION__);
  return NULL;
}

int timeline_init(struct timeline *ctx)
{
  list_head_init(&ctx->by_slice);
  slist_uid_head_init(&ctx->by_track);
  return 0;
}

void timeline_release(struct timeline *ctx)
{
  list_head_release(&ctx->by_slice);
  slist_uid_head_release(&ctx->by_track);
}

int timeline_add_track(struct timeline *ctx,
		       struct timeline_track *track,
		       struct input *input)
{
  struct input_n3 *input_n3;
  struct timeline_slice *slice;
  struct timeline_track_n3 *track_n3;
  struct timeline_slice_n3 *slice_n3;

  /* 0) Add track to timeline */
  __slist_push__(&ctx->by_track, track);
  
  /* 1) Read input */
  while((input_n3 = input_read(input)) != NULL){
    /* 2) Create slice if necessary & sort it */
    PR_DBG("2) Create slice if necessary & sort it\n");
    if((slice = timeline_get_slice_anyway(ctx, input_n3->time)) != NULL){
      /* 3) Create track n3, register slice */
      PR_DBG("3) Create track n3, register slice\n");
      __try__(!timeline_track_n3_alloc(track_n3, input_n3, track, slice), err);
      __try__(timeline_track_add_track_n3(track, track_n3) < 0, next);
      /* 4) Create slice n3 & register track n3 */
      PR_DBG("4) Create slice n3 & register track n3\n");
      __try__(!timeline_slice_n3_alloc(slice_n3, track_n3), err);
      __slist_push__(&slice->slist_slice_n3s, slice_n3);
    __catch__(next):
      PR_DBG("5) Back to 1\n");
    }
  }

  return 0;

 __catch__(err):
  return -1;
}

int timeline_run_and_sync(struct timeline *ctx)
{
  /* Execute tracks one at a time */
  struct timeline_track *track;
  __slist_for_each__(&ctx->by_track, track){
    /* Run each track sequentially */
    struct timeline_track_n3 *track_n3;
    __list_for_each__(&track->list_track_n3s, track_n3){
      /* On each track_n3, run all indicators */
      struct indicator *indicator;
      __slist_for_each__(&track->slist_uid_indicators, indicator)
	indicator_feed(indicator, track_n3);
    }
  }
  
  return 0;
}
