
#include "sup_res.h"

static int sup_res_feed(struct indicator *i, struct timeline_entry *e) {

  struct sup_res *s = __indicator_self__(i);
  struct candle *candle = __timeline_entry_self__(e);

  /* TODO */
  return -1;
}

static void sup_res_reset(struct indicator *i) {

  /* ? */
}

int sup_res_init(struct sup_res *s, indicator_id_t id,
		 int step, candle_value_t cvalue) {

  __indicator_super__(s, sup_res_feed, sup_res_reset);
  /* In */
  s->step = step;
  s->cvalue = cvalue;
  
  return 0;
}

void sup_res_release(struct sup_res *s) {

  /* TODO */
}
