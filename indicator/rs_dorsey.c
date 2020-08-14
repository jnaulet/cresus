/*
 * Cresus EVO - rs_dorsey.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_dorsey.h"

static int rs_dorsey_feed(struct indicator *i, struct timeline_track_n3 *e)
{
  double ratio, value;
  struct rs_dorsey_n3 *n3;
  struct rs_dorsey *ctx = (void*)i;
  struct timeline_track_n3 *ref;

  /* Get synced track_n3 */
  if(!(ref = timeline_slice_get_track_n3(e->slice, ctx->ref_track_uid)))
    goto err;
  
  ratio = e->close / ref->close * 100.0;
  value = ratio / ctx->ratio_prev - 1.0; /* Ref is 0 */
  
  if(!i->is_empty){
    if(rs_dorsey_n3_alloc(n3, i, value))
      timeline_track_n3_add_indicator_n3(e, __indicator_n3__(n3));
  }
  
  /* Update ratio & diff for next round */
  ctx->ratio_prev = ratio;
  return 0;

 err:
  return -1;
}

static void rs_dorsey_reset(struct indicator *i)
{
  struct rs_dorsey *ctx = (void*)i;
  /* Nothing to do */
}

int rs_dorsey_init(struct rs_dorsey *ctx, unique_id_t uid,
                   unique_id_t ref_track_uid)
{
  /* init() */
  __indicator_init__(ctx, uid, rs_dorsey_feed, rs_dorsey_reset);
  __indicator_set_string__(ctx, "rsd[]");
  
  ctx->ref_track_uid = ref_track_uid;
  ctx->ratio_prev = 0.0;
  return 0;
}

void rs_dorsey_release(struct rs_dorsey *ctx)
{
  __indicator_release__(ctx);
}
