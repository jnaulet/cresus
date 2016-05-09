/*
 * Cresus EVO - rs_mansfield.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_mansfield.h"
#include "engine/candle.h"
#include "framework/verbose.h"

/*
 * RSM = ((RSD(today) / mma(RSD(today), n)) - 1) * 100.0
 */

static int rs_mansfield_feed(struct indicator *i, struct timeline_entry *e) {

  struct timeline_entry *entry;
  struct candle *c = __timeline_entry_self__(e);
  struct rs_mansfield *r = __indicator_self__(i);

  /* TODO : make this simpler */
  
  if(i->is_empty)
    /* Increment in list */
    r->ref_ptr = r->ref->next;
  
  if(list_is_head(r->ref_ptr)){
    PR_ERR("timeline_entry ref list is empty\n");
    goto err;
  }
  
  if((entry = timeline_entry_find(__list_self__(r->ref_ptr), e->time))){
    struct candle *cref = __timeline_entry_self__(entry);
    double rsd = c->close / cref->close;
    double last = average_value(&r->mma);
    double mma = average_update(&r->mma, rsd);
    if(average_is_available(&r->mma)){
      /* Finally set value */
      double value = ((rsd / mma) - 1) * 100.0;
      struct rs_mansfield_entry *rsm;
      if(rs_mansfield_entry_alloc(rsm, i, value, (mma - last)))
	candle_add_indicator_entry(c, __indicator_entry__(rsm));
    }
    
    /* Set new ref */
    r->ref_ptr = __list__(entry);
    return 0;
  }
  
 err:
  return -1;
}

static void rs_mansfield_reset(struct indicator *i) {

  struct rs_mansfield *r = __indicator_self__(i);
  /* Reset insights */
  average_reset(&r->mma);
  r->value = 0.0;
}

int rs_mansfield_init(struct rs_mansfield *r, indicator_id_t id, int period,
		      list_head_t(struct timeline_entry) *ref) {

  __indicator_super__(r, id, rs_mansfield_feed, rs_mansfield_reset);
  __indicator_set_string__(r, "rsm[%d]", period);
  
  average_init(&r->mma, AVERAGE_MATH, period);
  
  r->ref = ref;
  r->ref_ptr = ref;
  r->value = 0.0;
  
  return 0;
}

void rs_mansfield_release(struct rs_mansfield *r) {

  __indicator_release__(r);
  average_release(&r->mma);
}

/* Indicator-specific */
double rs_mansfield_value(struct rs_mansfield *r) {

  return average_value(&r->mma);
}
