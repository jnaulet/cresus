/*
 * Cresus TypesExperiment - track.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 07/27/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>

#include "framework/types.h"
#include "framework/track.h"
#include "framework/verbose.h"
#include "framework/indicator.h"

/*
 * Timeline track entry object
 */

int track_n3_init(struct track_n3 *ctx,
                  struct quotes_n3 *quotes_n3,
                  struct track *track)
{
  /* Internals */
  ctx->quotes = quotes_n3;
  ctx->balance_sheet.quarterly = NULL;
  ctx->balance_sheet.yearly = NULL;
  ctx->income_statement.quarterly = NULL;
  ctx->income_statement.yearly = NULL;
  ctx->dividends = NULL;
  ctx->splits = NULL;
  plist_head_init(&ctx->plist_indicator_n3s);
  ctx->track = track;
  /* General */
  ctx->time = quotes_n3->time; /* FIXME */
  return 0;
}

void track_n3_release(struct track_n3 *ctx)
{
  plist_head_release(&ctx->plist_indicator_n3s);
}

const char *track_n3_str(struct track_n3 *ctx)
{
  static char buf[256];
  return track_n3_str_r(ctx, buf);
}

const char *track_n3_str_r(struct track_n3 *ctx,
                           char *buf)
{
  struct quotes_n3 *quotes = ctx->quotes;
  sprintf(buf, "%s: %s o%.2f c%.2f h%.2f l%.2f v%.0f",
          ctx->track->name, time_to_iso8601(ctx->time),
          quotes->open, quotes->close, quotes->high,
          quotes->low, quotes->volume);
  
  return buf;
}

struct indicator_n3 *
track_n3_get_indicator_n3(struct track_n3 *ctx,
                          unique_id_t indicator_uid)
{
  struct plist *p;
  plist_for_each(&ctx->plist_indicator_n3s, p){
    struct indicator_n3 *n3 = p->ptr;
    if(n3->indicator->uid == indicator_uid)
      return n3;
  }
  
  return NULL;
}

struct track_n3 *
track_n3_prev(struct track_n3 *ctx, int n)
{
  struct plist *p = ctx->plist;
  
  do {
    /* Beware : if not enough data, will send 1st one again & again */
    if(p->ptr){
      ctx = (struct track_n3*)p->ptr;
      p = p->prev;
    }
  } while(n--);
  
  return ctx;
}

/*
 * Track object
 */

static int track_add_quotes(struct track *ctx, struct quotes *quotes)
{
  struct list *l;
  struct plist *p;
  struct track_n3 *track_n3;
  
  /* Fill-in with quotes values */
  list_for_each(&quotes->list_quotes_n3s, l){
    struct quotes_n3 *quotes_n3 = (void*)l;
    __try__(!track_n3_alloc(track_n3, quotes_n3, ctx), err);
    __try__(!plist_alloc(p, track_n3), err);
    /* Remember plist parent */
    track_n3->plist = p;
    /* Register track n3 */
    plist_add_tail(&ctx->plist_track_n3s, p);
  }
  
  return 0;
  
 __catch__(err):
  return -1;
}

int track_init(struct track *ctx, unique_id_t uid,
               const char *name, struct quotes *quotes)
{
  ctx->uid = uid;
  strncpy(ctx->name, name, sizeof(ctx->name));
  plist_head_init(&ctx->plist_track_n3s);
  plist_head_init(&ctx->plist_indicators);
  ctx->amount = 0;
  ctx->transaction_fee = 0;
  ctx->private = NULL;
  return track_add_quotes(ctx, quotes);
}

void track_release(struct track *ctx)
{
  plist_head_release(&ctx->plist_track_n3s);
  plist_head_release(&ctx->plist_indicators);
}

int track_add_balance_sheet(struct track *ctx, struct balance_sheet *b)
{
  struct plist *p;
  struct balance_sheet_n3 *n3 = NULL;
  struct balance_sheet_n3 *next = (void*)list_next_null(&b->list_balance_sheet_n3s);

  /* Useless to continue */
  if(!next)
    return -1;
  
  /* Fill-in with balance_sheet values */
  plist_for_each(&ctx->plist_track_n3s, p){
    struct track_n3 *track_n3 = p->ptr;
    if(next && timecmp(track_n3->time, next->time) >= 0){
      n3 = next;
      next = (void*)list_next_null(&next->list);
      PR_DBG("%s new at %s\n", __FUNCTION__, time_to_iso8601(n3->time));
    }
    
    track_n3->balance_sheet.period[b->period] = n3;
  }
  
  return 0;
}

int track_add_income_statement(struct track *ctx, struct income_statement *i)
{
  struct plist *p;
  struct income_statement_n3 *n3 = NULL;
  struct income_statement_n3 *next = (void*)list_next_null(&i->list_income_statement_n3s);

  /* Useless to continue */
  if(!next)
    return -1;
  
  /* Fill-in with income_statement values */
  plist_for_each(&ctx->plist_track_n3s, p){
    struct track_n3 *track_n3 = p->ptr;
    if(next && timecmp(track_n3->time, next->time) >= 0){
      n3 = next;
      next = (void*)list_next_null(&next->list);
      PR_DBG("%s new at %s\n", __FUNCTION__, time_to_iso8601(n3->time));
    }
    
    track_n3->income_statement.period[i->period] = n3;
  }

  return 0;
}

int track_add_cash_flow(struct track *ctx, struct cash_flow *c)
{
  struct plist *p;
  struct cash_flow_n3 *n3 = NULL;
  struct cash_flow_n3 *next = (void*)list_next_null(&c->list_cash_flow_n3s);

  /* Useless to continue */
  if(!next)
    return -1;
  
  /* Fill-in with cash_flow values */
  plist_for_each(&ctx->plist_track_n3s, p){
    struct track_n3 *track_n3 = p->ptr;
    if(next && timecmp(track_n3->time, next->time) >= 0){
      n3 = next;
      next = (void*)list_next_null(&next->list);
      PR_DBG("%s new at %s\n", __FUNCTION__, time_to_iso8601(n3->time));
    }
    
    track_n3->cash_flow.period[c->period] = n3;
  }

  return 0;
}

int track_add_dividends(struct track *ctx, struct dividends *d)
{
  struct list *l;
  struct plist *p;
  
  /* Fill-in with balance_sheet values */
  plist_for_each(&ctx->plist_track_n3s, p){
    struct track_n3 *track_n3 = p->ptr;
    /* TODO: optimize */
    list_for_each(&d->list_dividends_n3s, l){
      struct dividends_n3 *dividends_n3 = (void*)l;
      if(!timecmp(track_n3->time, dividends_n3->time))
	track_n3->dividends = dividends_n3;
    }
  }

  return 0;
}

int track_add_splits(struct track *ctx, struct splits *s)
{
  struct list *l;
  struct plist *p;
  
  /* Fill-in with balance_sheet values */
  plist_for_each(&ctx->plist_track_n3s, p){
    struct track_n3 *track_n3 = p->ptr;
    /* TODO: optimize */
    list_for_each(&s->list_splits_n3s, l){
      struct splits_n3 *splits_n3 = (void*)l;
      if(!timecmp(track_n3->time, splits_n3->time))
	track_n3->splits = splits_n3;
    }
  }
  
  return 0;
}
