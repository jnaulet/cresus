/*
 * Cresus EVO - rs_mansfield.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef RS_MANSFIELD_H
#define RS_MANSFIELD_H

/*
 * Relative Strength (Mansfield)
 * Fomula :
 * RSM = ((RSD(today) / mma(RSD(today), n)) - 1) * 100.0
 */

#include <stdlib.h>
#include "math/average.h"
#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

/* Entries */

struct rs_mansfield_entry {
  /* As below */
  __inherits_from_indicator_entry__;
  /* Single value */
  double value;
  /* More info */
  double direction;
  /* Events ? */
};

#define rs_mansfield_entry_alloc(entry, parent, value, direction)	\
  DEFINE_ALLOC(struct rs_mansfield_entry, entry,			\
	       rs_mansfield_entry_init, parent, value,			\
	       direction)
#define rs_mansfield_entry_free(entry)			\
  DEFINE_FREE(entry, rs_mansfield_entry_release)

static inline int rs_mansfield_entry_init(struct rs_mansfield_entry *entry,
					  struct indicator *parent,
					  double value, double direction){
  __indicator_entry_super__(entry, parent);
  entry->value = value;
  entry->direction = direction;
  return 0;
}

static inline void
rs_mansfield_entry_release(struct rs_mansfield_entry *entry) {
  __indicator_entry_release__(entry);
}

/* Object */

#define rs_mansfield_alloc(r, id, period, ref)				\
  DEFINE_ALLOC(struct rs_mansfield, r, rs_mansfield_init, id, period, ref)
#define rs_mansfield_free(r)			\
  DEFINE_FREE(r, rs_mansfield_release)

struct rs_mansfield {
  /* As always, inherits from indicator */
  __inherits_from_indicator__;
  
  struct average mma;
  __list_head__(struct timeline_entry) *ref;

  double value;
};

int rs_mansfield_init(struct rs_mansfield *r, indicator_id_t id, int period,
		      __list_head__(struct timeline_entry) *ref);
void rs_mansfield_release(struct rs_mansfield *r);

/* Indicator-specific */
double rs_mansfield_value(struct rs_mansfield *r);

#endif
