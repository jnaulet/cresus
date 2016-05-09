#ifndef SUP_RES_H
#define SUP_RES_H

#include "engine/candle.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

struct sup_res_entry {
  /* As always */
  __inherits_from_indicator_entry__;
  /* Self */
  double value;
};

static inline int sup_res_entry_init(struct sup_res_entry *entry,
				     struct indicator *parent,
				     double value) {
  __indicator_entry_super__(entry, parent);
  entry->value = value;
  return 0;
}

static inline void sup_res_entry_release(struct sup_res_entry *entry) {
  __indicator_entry_release__(entry);
}

/* Indicator */

#define sup_res_alloc(s, id, step, cvalue)				\
  DEFINE_ALLOC(struct sup_res, s, sup_res_init, id, step, cvalue)
#define sup_res_free(s)				\
  DEFINE_FREE(s, sup_res_release);

struct sup_res {
  /* As usual */
  __inherits_from_indicator__;
  /* Params */
  int step;
  candle_value_t cvalue;
};

int sup_res_init(struct sup_res *s, indicator_id_t id,
		 int step, candle_value_t cvalue);
void sup_res_release(struct sup_res *s);


#endif
