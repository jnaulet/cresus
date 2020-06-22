#ifndef SUP_RES_H
#define SUP_RES_H

#include "engine/candle.h"

#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

struct sup_res_n3 {
  /* As always */
  struct indicator_n3 indicator_n3;
  /* Self */
  double value;
};

static inline int sup_res_n3_init(struct sup_res_n3 *n3,
				     struct indicator *parent,
				     double value)
{
  indicator_n3_init(&n3->indicator_n3, parent);
  n3->value = value;
  return 0;
}

static inline void sup_res_n3_release(struct sup_res_n3 *n3)
{
  indicator_n3_release(&n3->indicator_n3);
}

/* Indicator */

#define sup_res_alloc(s, id, step, cvalue)				\
  DEFINE_ALLOC(struct sup_res, s, sup_res_init, id, step, cvalue)
#define sup_res_free(s)				\
  DEFINE_FREE(s, sup_res_release);

struct sup_res {
  /* As usual */
  struct indicator indicator;
  /* Params */
  int step;
  candle_value_t cvalue;
};

int sup_res_init(struct sup_res *s, unique_id_t id,
		 int step, candle_value_t cvalue);
void sup_res_release(struct sup_res *s);

#endif
