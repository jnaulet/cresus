/*
 * Cresus EVO - rs_roc.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 15/10/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_roc.h"
#include "framework/verbose.h"
#include "framework/timeline_v2.h"

static int rs_roc_feed(struct indicator *i, struct track_n3 *e)
{
  struct rs_roc_n3 *n3;
  double value, roc, roc_ref;
  
  struct rs_roc *ctx = (void*)i;
  struct track_n3 *ref;
  struct track_n3 *last;
  struct track_n3 *ref_last;
  
  /* Get synced track_n3 */
  if(!(ref = slice_get_track_n3(e->slice, ctx->ref_track_uid)))
    goto out;
  
  /* Get relative n3 */
  last = track_n3_prev(e, ctx->period);
  /* Get relative n3 to ext synced */
  ref_last = track_n3_prev(ref, ctx->period);
  
  /* ROC formula :
   * ((candle[n] / candle[n - period]) - 1) * 100.0
   */
  roc = (e->quotes->close / last->quotes->close - 1) * 100.0;
  roc_ref = (ref->quotes->close / ref_last->quotes->close - 1) * 100.0;
  value = roc - roc_ref;
  
  if(rs_roc_n3_alloc(n3, i, value, roc, roc_ref)){
    track_n3_add_indicator_n3(e, &n3->indicator_n3);
    return 0;
  }
  
 out:
  return -1;
}

static void rs_roc_reset(struct indicator *i)
{
}

int rs_roc_init(struct rs_roc *ctx, unique_id_t uid, int period,
                unique_id_t ref_track_uid)
{
  /* Super() */
  indicator_init(&ctx->indicator, uid, rs_roc_feed, rs_roc_reset);
  indicator_set_string(&ctx->indicator, "rs_roc[%d]", period);
  
  ctx->ref_track_uid = ref_track_uid;
  ctx->period = period;
  return 0;
}

void rs_roc_release(struct rs_roc *ctx)
{
  indicator_release(&ctx->indicator);
}
