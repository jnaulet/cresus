/*
 * Cresus EVO - rs_mansfield.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "rs_mansfield.h"

/*
 * RSM = ((RSD(today) / mma(RSD(today), n)) - 1) * 100.0
 */

static int rs_mansfield_feed(struct indicator *i, struct candle *c) {

  struct timeline_entry *entry;
  time_t time = __timeline_entry__(c)->time;
  struct rs_mansfield *r = __indicator_self__(i);

  /* First data */
  if(i->is_empty){
    r->ref = c;
    goto out;
  }
  
  if((entry = __timeline_entry_find__(r->ref, time))){
    double rsd, mma;
    r->ref = __timeline_entry_self__(entry);
    rsd = c->close / r->ref->close;
    mma = average_update(&r->mma, rsd);
    if(average_is_available(&r->mma))
      /* Finally set value */
      r->value = ((rsd / mma) - 1) * 100.0;
  }

 out:
  return 0;
}

int rs_mansfield_init(struct rs_mansfield *r,
		      int period, struct candle *ref) {

  __indicator_super__(r,  rs_mansfield_feed);
  __indicator_set_string__(r, "rsm[%d]", period);
  
  average_init(&r->mma, AVERAGE_MATH, period);
  
  r->ref = ref;
  r->value = 0.0;
  
  return 0;
}

void rs_mansfield_free(struct rs_mansfield *r) {

  __indicator_free__(r);
  average_free(&r->mma);
}

/* Indicator-specific */
double rs_mansfield_value(struct rs_mansfield *r) {

  return average_value(&r->mma);
}
