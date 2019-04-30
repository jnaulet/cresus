/*
 * Cresus EVO - rs_roc.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 15/10/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_roc.h"
#include "framework/verbose.h"

static int rs_roc_feed(struct indicator *i, struct timeline_track_n3 *e)
{
  double value, roc, roc_ref;
  struct rs_roc_n3 *n3;

  struct rs_roc *ctx = (void*)i;
  struct timeline_track_n3 *ref;
  struct timeline_track_n3 *last;
  struct timeline_track_n3 *ref_last;
  
  /* Get relative n3 */
  if(!(last = __list_relative__(e, -(ctx->period))))
    goto out;
  /* Get synced track_n3 */
  if(!(ref = timeline_slice_get_track_n3(e->slice, ctx->ref_track_uid)))
    goto out;
  /* Get relative n3 to ext synced */
  if(!(ref_last = __list_relative__(ref, -(ctx->period))))
    goto out;
  
  /* ROC formula :
   * ((candle[n] / candle[n - period]) - 1) * 100.0
   */
  roc = (e->close / last->close - 1) * 100.0;
  roc_ref = (ref->close / ref_last->close - 1) * 100.0;
  value = roc - roc_ref;
    
  if(rs_roc_n3_alloc(n3, i, value, roc, roc_ref)){
    timeline_track_n3_add_indicator_n3(e, __indicator_n3__(n3));
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
  __indicator_init__(ctx, uid, rs_roc_feed, rs_roc_reset);
  __indicator_set_string__(ctx, "rs_roc[%d]", period);

  ctx->ref_track_uid = ref_track_uid;
  ctx->period = period;
  return 0;
}

void rs_roc_release(struct rs_roc *ctx)
{
  __indicator_release__(ctx);
}
