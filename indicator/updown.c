/*
 * Cresus EVO - updown.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/10/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "updown.h"
#include <string.h>

static int updown_feed(struct indicator *i, struct timeline_entry *e) {
  
  struct updown *u = __indicator_self__(i);
  
  /* FIXME : what if s1 == s2 or s0 == s1 ? */
  struct list *l2 = list_prev_n(__list__(e), 2);
  struct list *l1 = list_prev_n(__list__(e), 1);
  struct list *l0 = __list__(e);

  /* No list head */
  if(!list_is_head(l2) && !list_is_head(l1)){
    struct candle *c2 = __timeline_entry_self__((struct timeline_entry*)__list_self__(l2));
    struct candle *c1 = __timeline_entry_self__((struct timeline_entry*)__list_self__(l1));
    struct candle *c0 = __timeline_entry_self__((struct timeline_entry*)__list_self__(l0));

    /* Real values */
    double v2 = candle_get_value(c2, u->cvalue);
    double v1 = candle_get_value(c1, u->cvalue);
    double v0 = candle_get_value(c0, u->cvalue);
    
    /* Detect type */
    if(v0 < v1 && v1 > v2){
      /* UPDOWN_TOP */
      u->type = UPDOWN_TOP;
    }

    if(v0 > v1 && v1 < v2){
      /* UPDOWN_BOTTOM */
      u->type = UPDOWN_BOTTOM;
    }
  }
  
  return 0;
}

static void updown_reset(struct indicator *i) {

  struct updown *u = __indicator_self__(i);
  /* RAZ */
}

int updown_init(struct updown *u, indicator_id_t id, candle_value_t cvalue) {
  
  /* Super() */
  __indicator_super__(u, id, updown_feed, updown_reset);
  __indicator_set_string__(u, "updown");
  
  u->type = UPDOWN_NONE;
  u->cvalue = cvalue;

  return 0;
}

void updown_release(struct updown *u)
{
  __indicator_release__(u);
}
