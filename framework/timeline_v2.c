/*
 * Cresus TypesExperiment - timeline_v2.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/05/2019
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <libgen.h>

#include "framework/types.h"
#include "framework/verbose.h"
#include "framework/indicator.h"
#include "framework/timeline_v2.h"

/*
 * Timeline slice object
 */
int slice_init(struct slice *ctx, time64_t time)
{
  ctx->time = time;
  plist_head_init(&ctx->plist_track_n3s);
  return 0;
}

void slice_release(struct slice *ctx)
{
  plist_head_release(&ctx->plist_track_n3s);
}
                            
struct track_n3*
slice_get_track_n3(struct slice *ctx, unique_id_t uid)
{
  struct plist *p;
  plist_for_each(&ctx->plist_track_n3s, p){
    struct track_n3 *n3 = p->ptr;
    if(n3->track->uid == uid) return n3;
  }
  
  return NULL;
}

/*
 * Timeline object
 */

static struct slice *
timeline_v2_get_slice_anyway(struct timeline_v2 *ctx, time64_t time)
{
  struct plist *p;
  struct slice *slice;
  
  plist_for_each(&ctx->by_slice, p){
    struct slice *ptr = p->ptr;
    time64_t cmp = TIME64CMP(ptr->time, time, GR_DAY);
    /* Slice already exists, we go out */
    if(!cmp){
      PR_DBG("timeline_v2.c: slice already exists\n");
      slice = ptr;
      goto out;
    }
    /* Slice is ahead, sort */
    if(cmp > 0){
      __try__(!slice_alloc(slice, time), err);
      plist_add_tail_ptr(p, slice); /* Insert before */
      /* Debug */
      static char buf0[12], buf1[12];
      PR_DBG("timeline_v2.c: slice %s is missing, insertion before %s\n",
	     time64_str_r(time, GR_DAY, buf0),
	     time64_str_r(ptr->time, GR_DAY, buf1));
      /* Jump outside anyway */
      goto out;
    }
  }
  
  /* Slice doesn't exist, we create it */
  __try__(!slice_alloc(slice, time), err);
  plist_add_tail_ptr(&ctx->by_slice, slice);
  
  /* Debug */
  PR_DBG("timeline_v2.c: new slice at %s\n", time64_str(time, GR_DAY));
  
 out:
  return slice;
  
 __catch__(err):
  PR_ERR("%s: can't allocate slice object\n", __FUNCTION__);
  return NULL;
}

int timeline_v2_init(struct timeline_v2 *ctx)
{
  plist_head_init(&ctx->by_slice);
  plist_head_init(&ctx->by_track);
  return 0;
}

/* FIXME */
static double amount = 0.0;
static double transaction_fee = 0.0;

int timeline_v2_init_ex(struct timeline_v2 *ctx,
			int argc, char **argv,
			struct timeline_v2_ex_interface *itf)
{
  unique_id_t track_uid = 0;
  struct track *track = NULL;

  struct quotes *quotes;
  struct balance_sheet *balance;

  /* Base */
  timeline_v2_init(ctx);

  for(int i = 0; i < argc; i++){
    /* Quotes */
    if(!strcmp(argv[i], "--track")){
      char *filename = argv[++i];
      quotes_alloc(quotes, filename, NULL);
      track_alloc(track, track_uid++, basename(filename), quotes, NULL);
      /* Callback for indicators here */
      if(itf && itf->customize_track)
	itf->customize_track(ctx, track);
      /* Add to timeline_v2 */
      timeline_v2_add_track(ctx, track);
      /* Set fees & amount ? */
      track->transaction_fee = transaction_fee;
      track->amount = amount;
      continue;
    }
    /* Balance sheet */
    if(!strcmp(argv[i], "--balance-sheet")){
      char *filename = argv[++i];
      balance_sheet_alloc(balance, filename, NULL);
      track_add_balance_sheet(track, balance);
      continue;
    }
    /* Fees */
    if(!strcmp(argv[i], "--fee")){
      sscanf(argv[++i], "%lf", &transaction_fee);
      if(track) track->transaction_fee = transaction_fee;
      continue;
    }
    /* Fixed amount */
    if(!strcmp(argv[i], "--amount")){
      sscanf(argv[++i], "%lf", &amount);
      if(track) track->amount = amount;
      continue;
    }
    /* Unknown command */
    if(itf && itf->custom_opt)
      itf->custom_opt(ctx, argv[i], argv[i+1]);
  }

  return 0;
}

void timeline_v2_release(struct timeline_v2 *ctx)
{
  plist_head_release(&ctx->by_slice);
  plist_head_release(&ctx->by_track);
}

int timeline_v2_add_track(struct timeline_v2 *ctx,
		       struct track *track)
{
  struct plist *p, *q;
  struct slice *slice;
  struct track_n3 *track_n3;

  /* Add track to timeline_v2 */
  plist_add_ptr(&ctx->by_track, track);
  
  /* Read input */
  plist_for_each(&track->plist_track_n3s, p){
    struct track_n3 *track_n3 = p->ptr;
    /* Create slice if necessary & sort it */
    if((slice = timeline_v2_get_slice_anyway(ctx, track_n3->time)) != NULL){
      /* Register track n3 into slice */
      track_n3->slice = slice; /* Put marker */
      plist_add_ptr(&slice->plist_track_n3s, track_n3); /* any end should do */
      /* Execute all indicators */
      plist_for_each(&track->plist_indicators, q){
	struct indicator *indicator = q->ptr;
	indicator_feed(indicator, track_n3);
      }
    }
  }
  
  return 0;
  
 __catch__(err):
  return -1;
}

struct track *
timeline_v2_find_track_by_uid(struct timeline_v2 *ctx,
			      unique_id_t uid)
{
  struct plist *p;
  plist_for_each(&ctx->by_track, p){
    struct track *t = p->ptr;
    if(t->uid == uid) return (struct track*)p->ptr;
  }
  
  return NULL;
}
