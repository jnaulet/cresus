/*
 * Cresus EVO - rs_dorsey.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_dorsey.h"
#include "engine/candle.h"

static int rs_dorsey_feed(struct indicator *i, struct timeline_entry *e)
{
  struct timeline_entry *ref_entry;
  struct rs_dorsey *ctx = __indicator_self__(i);
  struct candle *c = __timeline_entry_self__(e);
 
  if((ref_entry = timeline_entry_find(__list_self__(ctx->ref), e->time))){
    struct rs_dorsey_entry *entry;
    struct candle *ref_c = __timeline_entry_self__(ref_entry);
    double ratio = c->close / ref_c->close * 100.0;
    double value = ratio / ctx->ratio_prev - 1.0; /* Ref is 0 */
    
    if(!i->is_empty){
      if(rs_dorsey_entry_alloc(entry, i, value))
	candle_add_indicator_entry(c, __indicator_entry__(entry));
      /* Set new ref for better performance */
      ctx->ref = __list__(ref_entry);
    }

    /* Update ratio & diff for next round */
    ctx->ratio_prev = ratio;
    return 0;
  }

  return -1;
}

static void rs_dorsey_reset(struct indicator *i)
{
  struct rs_dorsey *ctx = __indicator_self__(i);
  /* Nothing to do */
}

int rs_dorsey_init(struct rs_dorsey *ctx, indicator_id_t id,
		   list_head_t(struct timeline_entry) *ref)
{
  /* super() */
  __indicator_super__(ctx, id, rs_dorsey_feed, rs_dorsey_reset);
  __indicator_set_string__(ctx, "rsd[]");
  
  ctx->ref = ref->next;
  ctx->ratio_prev = 0.0;
  return 0;
}

void rs_dorsey_release(struct rs_dorsey *ctx)
{
  __indicator_release__(ctx);
}
