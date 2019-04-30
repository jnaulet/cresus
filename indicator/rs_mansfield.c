/*
 * Cresus EVO - rs_mansfield.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_mansfield.h"
#include "framework/verbose.h"

/*
 * RSM = ((RSD(today) / mma(RSD(today), n)) - 1) * 100.0
 */

static int rs_mansfield_feed(struct indicator *i,
                             struct timeline_track_n3 *e)
{
  struct timeline_track_n3 *ref;
  struct timeline_track_n3 *n3;
  struct rs_mansfield *ctx = (void*)i;
  
  /* Get synced track_n3 */
  if(!(ref = timeline_slice_get_track_n3(e->slice, ctx->ref_track_uid)))
    goto err;
  
  double rsd = e->close / ref->close;
  double last = average_value(&ctx->mma);
  double mma = average_update(&ctx->mma, rsd);
  if(average_is_available(&ctx->mma)){
    /* Finally set value */
    struct rs_mansfield_n3 *rsm;
    double value = ((rsd / mma) - 1) * 100.0;
    if(rs_mansfield_n3_alloc(rsm, i, value, (mma - last)))
      timeline_track_n3_add_indicator_n3(e, __indicator_n3__(rsm));
    
    return 0;
  }
  
 err:
  return -1;
}

static void rs_mansfield_reset(struct indicator *i)
{
  struct rs_mansfield *ctx = (void*)i;
  /* Reset insights */
  average_reset(&ctx->mma);
  ctx->value = 0.0;
}

int rs_mansfield_init(struct rs_mansfield *ctx, unique_id_t uid, int period,
                      unique_id_t ref_track_uid)
{
  __indicator_init__(ctx, uid, rs_mansfield_feed, rs_mansfield_reset);
  __indicator_set_string__(ctx, "rsm[%d]", period);
  
  average_init(&ctx->mma, AVERAGE_MATH, period);
  ctx->ref_track_uid = ref_track_uid;
  ctx->value = 0.0;
  
  return 0;
}

void rs_mansfield_release(struct rs_mansfield *ctx)
{
  __indicator_release__(ctx);
  average_release(&ctx->mma);
}

/* Indicator-specific */
double rs_mansfield_value(struct rs_mansfield *ctx)
{
  return average_value(&ctx->mma);
}
