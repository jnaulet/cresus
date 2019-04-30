/*
 * Cresus EVO - jtrend.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 02/05/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "jtrend.h"

static int jtrend_feed(struct indicator *i,
		       struct timeline_track_n3 *e)
{
  double value, ref_value;
  struct jtrend_n3 *n3;

  struct jtrend *ctx = (void*)i;
  struct timeline_track_n3 *ref;
  
  /* Get synced track_n3 */
  if(!(ref = timeline_slice_get_track_n3(e->slice, ctx->ref_track_uid)))
    goto err;
  
  if(roc_compute(&ctx->roc, e, &value) != -1){
    /* FIXME : check return */
    roc_compute(&ctx->roc_ref, ref, &ref_value);
    /* Alloc jn3 & store in candle */
    value = value - ref_value;
    if(jtrend_n3_alloc(n3, i, value, ref_value))
      timeline_track_n3_add_indicator_n3(e, __indicator_n3__(n3));

    return 0;
  }
  
 err:
  /* Do something here */
  return -1;
}

static void jtrend_reset(struct indicator *i)
{  
  struct jtrend *ctx = (void*)i;
  /* Reset sub-indicators */
  roc_reset(&ctx->roc);
  roc_reset(&ctx->roc_ref);
  /* TODO : what about ctx->ref ? */
}

int jtrend_init(struct jtrend *ctx, unique_id_t uid,
                int period, int average,
                unique_id_t ref_track_uid)
{
  __indicator_init__(ctx, uid, jtrend_feed, jtrend_reset);
  __indicator_set_string__(ctx, "jtrend[%d,%d]", period, average);
  
  /* Our sub-indicators */
  roc_init(&ctx->roc, uid, period, average);
  roc_init(&ctx->roc_ref, uid, period, average);
  
  ctx->ref_track_uid = ref_track_uid;
  return 0;
}

void jtrend_release(struct jtrend *ctx)
{  
  __indicator_release__(ctx);
}
