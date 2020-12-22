/*
 * Cresus TypesExperiment - timeline_v2.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/05/2019
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>

#include "framework/types.h"
#include "framework/verbose.h"
#include "framework/timeline_v2.h"

/*
 * Timeline slice object
 */

int slice_init(struct slice *ctx, time_t time)
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
timeline_v2_get_slice_anyway(struct timeline_v2 *ctx, time_t time)
{
  struct plist *p;
  struct slice *slice;
  
  plist_for_each(&ctx->by_slice, p){
    struct slice *ptr = p->ptr;
    time_t cmp = timecmp(ptr->time, time);
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
      PR_DBG("timeline_v2.c: slice %s is missing, insertion before %s\n",
             time_to_iso8601(time), time_to_iso8601(ptr->time));
      /* Jump outside anyway */
      goto out;
    }
  }
  
  /* Slice doesn't exist, we create it */
  __try__(!slice_alloc(slice, time), err);
  plist_add_tail_ptr(&ctx->by_slice, slice);
  
  /* Debug */
  PR_DBG("timeline_v2.c: new slice at %s\n", time_to_iso8601(time));
  
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

  /* Params */
  ctx->track_uid = 0;
  ctx->amount = 0.0;
  ctx->transaction_fee = 0.0;
  
  return 0;
}

static const char *basename(const char *filename)
{
  char *base = strrchr(filename, '/');
  return (base ? base + 1 : filename);
}

static struct track *
timeline_v2_init_ex_track(struct timeline_v2 *ctx,
                          const char *filename,
                          struct timeline_v2_ex_interface *itf)
{
  struct quotes *quotes;
  struct track *track = NULL;
  
  quotes_alloc(quotes, filename);
  track_alloc(track, ctx->track_uid++, basename(filename), quotes);
  /* Callback for indicators here */
  if(itf && itf->customize_track)
    itf->customize_track(ctx, track);
  /* Add to timeline_v2 */
  timeline_v2_add_track(ctx, track);

  /* Set fees & amount */
  track->transaction_fee = ctx->transaction_fee;
  track->amount = ctx->amount;
  
  /* FIXME */
  return track;
}

static int timeline_v2_init_ex_balance_sheet(struct timeline_v2 *ctx,
                                             struct track *track,
                                             const char *filename)
{
  struct balance_sheet *balance;
  
  /* Quarterly */
  balance_sheet_alloc(balance, filename, QUARTERLY);
  track_add_balance_sheet(track, balance);
  /* Yearly */
  balance_sheet_alloc(balance, filename, YEARLY);
  track_add_balance_sheet(track, balance);
  
  /* FIXME */
  return 0;
}

static int timeline_v2_init_ex_income_statement(struct timeline_v2 *ctx,
                                                struct track *track,
                                                const char *filename)
{
  struct income_statement *income;
  
  /* Quarterly */
  income_statement_alloc(income, filename, QUARTERLY);
  track_add_income_statement(track, income);
  /* Yearly */
  income_statement_alloc(income, filename, YEARLY);
  track_add_income_statement(track, income);
  
  /* FIXME */
  return 0;
}

static int timeline_v2_init_ex_cash_flow(struct timeline_v2 *ctx,
                                         struct track *track,
                                         const char *filename)
{
  struct cash_flow *cash;
  
  /* Quarterly */
  cash_flow_alloc(cash, filename, QUARTERLY);
  track_add_cash_flow(track, cash);
  /* Yearly */
  cash_flow_alloc(cash, filename, YEARLY);
  track_add_cash_flow(track, cash);
  
  /* FIXME */
  return 0;
}

static int timeline_v2_init_ex_dividends(struct timeline_v2 *ctx,
                                         struct track *track,
                                         const char *filename)
{
  struct dividends *dividends;
  dividends_alloc(dividends, filename);
  track_add_dividends(track, dividends);

  /* FIXME */
  return 0;
}

static int timeline_v2_init_ex_splits(struct timeline_v2 *ctx,
                                      struct track *track,
                                      const char *filename)
{
  struct splits *splits;
  splits_alloc(splits, filename);
  track_add_splits(track, splits);
  
  /* FIXME */
  return 0;
}

int timeline_v2_init_ex(struct timeline_v2 *ctx,
                        int argc, char **argv,
                        struct timeline_v2_ex_interface *itf)
{
  static struct track *track = NULL;
  
  /* Base */
  timeline_v2_init(ctx);

  for(int i = 0; i < argc; i++){
    /* Quotes */
    if(!strcmp(argv[i], "--track")){
      track = timeline_v2_init_ex_track(ctx, argv[++i], itf);
      continue;
    }
    /* Balance sheet */
    if(!strcmp(argv[i], "--balance-sheet")){
      timeline_v2_init_ex_balance_sheet(ctx, track, argv[++i]);
      continue;
    }
    /* Income statement */
    if(!strcmp(argv[i], "--income-statement")){
      timeline_v2_init_ex_income_statement(ctx, track, argv[++i]);
      continue;
    }
    /* Cash flow */
    if(!strcmp(argv[i], "--cash-flow")){
      timeline_v2_init_ex_cash_flow(ctx, track, argv[++i]);
      continue;
    }
    /* All in one */
    if(!strcmp(argv[i], "--fundamentals")){
      timeline_v2_init_ex_balance_sheet(ctx, track, argv[++i]);
      timeline_v2_init_ex_income_statement(ctx, track, argv[i]);
      timeline_v2_init_ex_cash_flow(ctx, track, argv[i]);
      continue;
    }
    /* Dividends */
    if(!strcmp(argv[i], "--dividends")){
      timeline_v2_init_ex_dividends(ctx, track, argv[++i]);
      continue;
    }
    /* Splits */
    if(!strcmp(argv[i], "--splits")){
      timeline_v2_init_ex_splits(ctx, track, argv[++i]);
      continue;
    }
    /* Fees */
    if(!strcmp(argv[i], "--fee") ||
       !strcmp(argv[i], "--fees")){
      sscanf(argv[++i], "%lf", &ctx->transaction_fee);
      if(track) track->transaction_fee = ctx->transaction_fee;
      continue;
    }
    /* Fixed amount */
    if(!strcmp(argv[i], "--amount")){
      sscanf(argv[++i], "%lf", &ctx->amount);
      if(track) track->amount = ctx->amount;
      continue;
    }
    /* Better name */
    if(!strcmp(argv[i], "--name")){
      strncpy(track->name, argv[++i], sizeof(track->name));
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
