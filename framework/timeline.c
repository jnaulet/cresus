/*
 * Cresus CommonTL - timeline.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/05/2019
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>

#include "framework/verbose.h"
#include "framework/timeline.h"
#include "framework/indicator.h"

static char buf[256];

/*
 * Timeline track object
 */
const char *timeline_track_n3_str(struct timeline_track_n3 *ctx)
{
  return timeline_track_n3_str_r(ctx, buf);
}

const char *timeline_track_n3_str_r(struct timeline_track_n3 *ctx,
                                    char *buf)
{
  sprintf(buf, "%s: %s " input_n3_interface_fmt,
          ctx->track->name,
          time64_str(ctx->slice->time, GR_DAY), /* ! */
          input_n3_interface_args(ctx));
  
  return buf;
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
  
  /* TODO : Remember last position ? */
  __list_for_each__(&ctx->by_slice, ptr){
    time64_t cmp = TIME64CMP(ptr->time, time, GR_DAY); /* ! */
    /* Slice already exists, we go out */
    if(!cmp){
      PR_WARN("timeline.c: slice already exists\n");
      goto out;
    }
    /* Slice is ahead, sort */
    if(cmp > 0){
      struct timeline_slice *slice;
      timeline_slice_alloc(slice, time);
      __list_add__(ptr, slice);
      PR_WARN("timeline.c: slice is missing, insertion\n");
      goto out;
    }
  }
  
  /* Slice doesn't exist, we create it */
  timeline_slice_alloc(ptr, time);
  __list_add_tail__(&ctx->by_slice, ptr);
  //PR_DBG("timeline.c: new slice at %s\n", time64_str(time, GR_DAY));
  
 out:
  return ptr;
}

int timeline_init(struct timeline *ctx)
{
  list_head_init(&ctx->by_slice);
  slist_head_init(&ctx->by_track);
  return 0;
}

void timeline_release(struct timeline *ctx)
{
  list_head_release(&ctx->by_slice);
  slist_head_release(&ctx->by_track);
}

int timeline_add_track(struct timeline *ctx,
		       struct timeline_track *track,
		       struct input *input)
{
  struct timeline_slice *slice;
  struct input_n3 *input_n3;
  struct timeline_track_n3 *track_n3;
  struct timeline_slice_n3 *slice_n3;

  /* 0) Add track to timeline */
  __slist_push__(&ctx->by_track, track);
  
  /* 1) Read input */
  while((input_n3 = input_read(input)) != NULL){
    /* 2) Create slice if necessary & sort it */
    //PR_DBG("2) Create slice if necessary & sort it\n");
    if((slice = timeline_get_slice_anyway(ctx, input_n3->time)) != NULL){
      /* 3) Create track n3, register slice */
      //PR_DBG("3) Create track n3, register slice\n");
      timeline_track_n3_alloc(track_n3, input_n3, track, slice); /* TODO : check return */
      __list_add_tail__(&track->list_track_n3s, track_n3); /* FIXME : sort this */
      /* 4) Create slice n3 & register track n3 */
      //PR_DBG("4) Create slice n3 & register track n3\n");
      timeline_slice_n3_alloc(slice_n3, track_n3); /* TODO : check return */
      __slist_push__(&slice->slist_slice_n3s, slice_n3);
      //PR_DBG("5) Back to 1\n");
    }
  }

  return 0;
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
      __slist_for_each__(&track->slist_indicators, indicator)
	indicator_feed(indicator, track_n3);
    }
  }
  
  return 0;
}
