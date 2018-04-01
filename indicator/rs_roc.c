/*
 * Cresus EVO - rs_roc.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 15/10/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_roc.h"
#include "framework/verbose.h"

static int rs_roc_feed(struct indicator *i, struct timeline_entry *e)
{
  struct timeline_entry *e_ref;
  struct rs_roc *ctx = __indicator_self__(i);
  
  struct candle *c = __timeline_entry_self__(e);
  struct timeline_entry *e_last = __list_relative__(e, -(ctx->period));

  /* Error check */
  if(!e_last) goto out;
  
  if((e_ref = timeline_entry_find(__list_self__(ctx->ref), e->time))){
    double value, roc, roc_ref;
    struct rs_roc_entry *entry;
    struct candle *c_ref = __timeline_entry_self__(e_ref);
    struct timeline_entry *e_ref_last = __list_relative__(e_ref, -(ctx->period));
    
    /* Error check */
    if(!e_ref_last) goto out;
    
    /* ROC formula :
     * ((candle[n] / candle[n - period]) - 1) * 100.0
     */
    roc = (c->close / ((struct candle*)__timeline_entry_self__(e_last))->close - 1) * 100.0;
    roc_ref = (c_ref->close / ((struct candle*)__timeline_entry_self__(e_ref_last))->close - 1) * 100.0;
    value = roc - roc_ref;
    
    if(rs_roc_entry_alloc(entry, i, value, roc, roc_ref)){
      candle_add_indicator_entry(c, __indicator_entry__(entry));
      // PR_INFO("rs_roc value: self %.2lf ref %.2lf value %.2lf\n",
      //      value, value_ref, entry->value);
    }

    /* Set new ref for better performance */
    ctx->ref = __list__(e_ref);
    return 0;
  }
  
 out:
  return -1;
}

static void rs_roc_reset(struct indicator *i)
{
}

int rs_roc_init(struct rs_roc *ctx, indicator_id_t id, int period,
		list_head_t(struct timeline_entry) *ref)
{
  /* Super() */
  __indicator_super__(ctx, id, rs_roc_feed, rs_roc_reset);
  __indicator_set_string__(ctx, "rs_roc[%d]", period);

  ctx->ref = ref->next;
  ctx->period = period;
  return 0;
}

void rs_roc_release(struct rs_roc *ctx)
{
  __indicator_release__(ctx);
}
