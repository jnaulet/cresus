/*
 * Cresus EVO - rs_dorsey.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_dorsey.h"

static int rs_dorsey_feed(struct indicator *i, struct candle *c) {

  struct timeline_entry *entry;
  time_t time = __timeline_entry__(c)->time;
  struct rs_dorsey *r = __indicator_self__(i);
  
  if((entry = __timeline_entry_find__(r->ref, time))){
    r->ref = __timeline_entry_self__(entry);
    r->value = c->close / r->ref->close;
    /* TODO : create new entry ? */
    return 0;
  }
  
  return -1;
}

int rs_dorsey_init(struct rs_dorsey *r, struct candle *ref) {

  /* super() */
  __indicator_super__(r, rs_dorsey_feed);
  __indicator_set_string__(r, "rsd[i]");

  r->ref = ref;
  return 0;
}

void rs_dorsey_free(struct rs_dorsey *r) {

  __indicator_free__(r);
}

/* Indicator-specific */
double rs_dorsey_value(struct rs_dorsey *r) {

  return r->value;
}
