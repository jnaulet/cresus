/*
 * Cresus EVO - jtrend.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 02/05/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "jtrend.h"

static int jtrend_feed(struct indicator *i,
		       struct timeline_entry *e) {

  struct jtrend_entry *jentry;
  struct timeline_entry *tentry;

  struct jtrend *j = __indicator_self__(i);
  struct candle *c = __timeline_entry_self__(e);

  if(!__list_self__(j->ref))
    j->ref = j->ref->next;
  
  if((tentry = timeline_entry_find(__list_self__(j->ref), e->time))){
    double value, average;
    struct jtrend_entry *entry;
    double ref_value, ref_average;

    if(roc_compute(&j->roc, e, &value, &average) != -1){
      /* FIXME : check return */
      roc_compute(&j->roc_ref, __timeline_entry_self__(tentry),
		  &ref_value, &ref_average);

      /* Alloc jentry & store in candle */
      value = value - ref_value;
      average = average - ref_average;
      if(jtrend_entry_alloc(entry, i, value, average))
	candle_add_indicator_entry(c, __indicator_entry__(entry));
    }

    j->ref = __list__(tentry);
    return 0;
  }

  /* Do something here */
  return -1;
}

int jtrend_init(struct jtrend *j, indicator_id_t id,
		int period, int average,
		list_head_t(struct timeline_entry) *ref) {

  __indicator_super__(j, id, jtrend_feed);
  /* Our sub-indicators */
  roc_init(&j->roc, id, period, average);
  roc_init(&j->roc_ref, id, period, average);

  j->ref = ref;

  return 0;
}

void jtrend_release(struct jtrend *j) {
  
  __indicator_release__(j);
}
