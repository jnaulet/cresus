/*
 * Cresus EVO - rs_roc.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 15/10/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef RS_ROC_H
#define RS_ROC_H

/*
 * Relative Strength / ROC comparison
 */

#include "engine/candle.h"
#include "framework/list.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

struct rs_roc_entry {
  /* As always... */
  __inherits_from_indicator_entry__;
  /* Data */
  double value;
  double roc, roc_ref;
};

#define rs_roc_entry_alloc(entry, parent, value, roc, roc_ref)		\
  DEFINE_ALLOC(struct rs_roc_entry, entry, rs_roc_entry_init, parent,	\
	       value, roc, roc_ref)
#define rs_roc_entry_free(entry)		\
  DEFINE_FREE(entry, rs_roc_entry_free)

static inline int rs_roc_entry_init(struct rs_roc_entry *entry,
				    struct indicator *parent,
				    double value, double roc, double roc_ref) {
  __indicator_entry_super__(entry, parent);
  entry->value = value;
  entry->roc =roc;
  entry->roc_ref = roc_ref;
  return 0;
}

static inline void rs_roc_entry_release(struct rs_roc_entry *entry) {
  __indicator_entry_release__(entry);
}

/* Main object */

#define rs_roc_alloc(ctx, id, period, ref)				\
  DEFINE_ALLOC(struct rs_roc, ctx, rs_roc_init, id, period, ref)
#define rs_roc_free(ctx)			\
  DEFINE_FREE(ctx, rs_roc_release)

struct rs_roc {
  /* Inherits from indicator */
  __inherits_from_indicator__;
  /* Internals */
  int period;
  list_head_t(struct timeline_entry) *ref;
};

int rs_roc_init(struct rs_roc *ctx, indicator_id_t id, int period,
		list_head_t(struct timeline_entry) *ref);
void rs_roc_release(struct rs_roc *ctx);

#endif
