/*
 * Cresus EVO - jtrend.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 02/05/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef JTREND_H
#define JTREND_H

#include "math/average.h"
#include "engine/candle.h"
#include "framework/alloc.h"
#include "framework/indicator.h"
/* Sub-indicators */
#include "indicator/roc.h"

struct jtrend_entry {
  /* Always */
  __inherits_from_indicator_entry__;
  /* Value type ? */
  double value;
  double average;
};

#define jtrend_entry_alloc(entry, parent, value, average)		\
  DEFINE_ALLOC(struct jtrend_entry, entry, jtrend_entry_init,		\
	       parent, value, average)
#define jtrend_entry_free(entry)		\
  DEFINE_FREE(entry, jtrend_entry_release)

static inline int jtrend_entry_init(struct jtrend_entry *entry,
				    struct indicator *parent,
				    double value, double average){
  __indicator_entry_super__(entry, parent);
  entry->value = value;
  entry->average = average;
  return 0;
}

static inline void jtrend_entry_release(struct jtrend_entry *entry) {
  __indicator_entry_release__(entry);
}

/* Main object */

#define jtrend_alloc(jtrend, id, period, average, ref)			\
  DEFINE_ALLOC(struct jtrend, jtrend, jtrend_init, id,			\
	       period, average, ref)
#define jtrend_free(jtrend)			\
  DEFINE_FREE(jtrend, jtrend_release)

struct jtrend {
  /* Anyway */
  __inherits_from_indicator__;
  /* Some other data */
  struct roc roc;
  struct roc roc_ref;
  list_head_t(struct timeline_entry) *ref;
};

int jtrend_init(struct jtrend *j, indicator_id_t id,
		int period, int average,
		list_head_t(struct timeline_entry) *ref);

void jtrend_release(struct jtrend *j);

#endif
