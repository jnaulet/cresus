
#include "sup_res.h"

static int sup_res_feed(struct indicator *i, struct track_n3 *e) {

  struct sup_res *s = (void*)i;
  struct candle *candle = (void*)e;

  /* TODO */
  return -1;
}

static void sup_res_reset(struct indicator *i) {

  /* ? */
}

int sup_res_init(struct sup_res *s, unique_id_t id,
		 int step, candle_value_t cvalue) {

  __indicator_init__(s, sup_res_feed, sup_res_reset);
  /* In */
  s->step = step;
  s->cvalue = cvalue;
  
  return 0;
}

void sup_res_release(struct sup_res *s) {

  /* TODO */
}
