/*
 * Cresus EVO - rs_dorsey.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_dorsey.h"
#include "engine/candle.h"

static int rs_dorsey_feed(struct indicator *i, struct timeline_entry *e) {

  struct timeline_entry *entry;
  struct rs_dorsey *r = __indicator_self__(i);
  struct candle *c = __timeline_entry_self__(e);

  if(i->is_empty)
    r->ref = __list__(e);
  
  if((entry = timeline_entry_find(__list_self__(r->ref), e->time))){
    struct rs_dorsey_entry *rsd;
    struct candle *cref = __timeline_entry_self__(entry);
    double value = c->close / cref->close;
    if(rs_dorsey_entry_alloc(rsd, i, value))
      candle_add_indicator_entry(c, __indicator_entry__(rsd));
    /* Set new ref */
    r->ref = __list__(entry);
    return 0;
  }
  
  return -1;
}

int rs_dorsey_init(struct rs_dorsey *r, indicator_id_t id,
		   list_head_t(struct timeline_entry) *ref) {

  /* super() */
  __indicator_super__(r, id, rs_dorsey_feed);
  __indicator_set_string__(r, "rsd[i]");

  r->ref = ref->next;
  return 0;
}

void rs_dorsey_release(struct rs_dorsey *r) {

  __indicator_release__(r);
}
