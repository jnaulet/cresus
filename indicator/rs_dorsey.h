/*
 * Cresus EVO - rs_dorsey.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef RS_DORSEY_H
#define RS_DORSEY_H

/*
 * Dorsey Relative Strength
 * Very simple formula :
 * RSD = (close / close_index) * 100.0
 */

#include "framework/alloc.h"
#include "framework/indicator.h"
#include "framework/timeline_entry.h"

/* Entries */

struct rs_dorsey_entry {
  /* As below */
  __inherits_from_indicator_entry__;
  /* Single value */
  double value;
};

#define rs_dorsey_entry_alloc(entry, parent, value)	\
  DEFINE_ALLOC(struct rs_dorsey_entry, entry,		\
	       rs_dorsey_entry_init, parent, value)
#define rs_dorsey_entry_free(entry)		\
  DEFINE_FREE(entry, rs_dorsey_entry_release);

static inline int rs_dorsey_entry_init(struct rs_dorsey_entry *entry,
				       struct indicator *parent,
				       double value) {
  __indicator_entry_super__(entry, parent);
  entry->value = value;
  return 0;
}

static inline void rs_dorsey_entry_release(struct rs_dorsey_entry *entry){
  __indicator_entry_release__(entry);
}

/* Maion object */

#define rs_dorsey_alloc(r, id, ref)				\
  DEFINE_ALLOC(struct rs_dorsey, r, rs_dorsey_init, id, ref)
#define rs_dorsey_free(r)			\
  DEFINE_FREE(r, rs_dorsey_release)

struct rs_dorsey {
  /* As always, inherits from indicator */
  __inherits_from_indicator__;
  /* Reference chart */
  list_head_t(struct timeline_entry) *ref;
  /* Internals */
  double ratio_prev;
};

int rs_dorsey_init(struct rs_dorsey *ctx, indicator_id_t id,
		   list_head_t(struct timeline_entry) *ref);
void rs_dorsey_release(struct rs_dorsey *ctx);

#endif
