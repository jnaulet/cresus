/*
 * Cresus EVO - heikin_ashi.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 01/26/2015
 * Copyright (c) 2015 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include "heikin_ashi.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static int heikin_ashi_feed(struct indicator *i, struct timeline_entry *e) {
  
  struct heikin_ashi *h = __indicator_self__(i);
  struct candle *c = __timeline_entry_self__(e);

  if(i->is_empty){
    /* First data */
    h->value.close = (c->open + c->close + c->high + c->low) / 4;
    h->value.open = c->open; /* FIXME : Arbitrary */
    h->value.high = c->high;
    h->value.low = c->low;
    goto out;
  }
  
  h->value.open = (h->value.close + h->value.open) / 2;
  h->value.close = (c->open + c->close + c->high + c->low) / 4;
  h->value.high = c->high;
  h->value.low = c->low;
  
  /* Define new direction */
  heikin_ashi_dir_t dir = ((h->value.open - h->value.close) < 0 ?
                           HEIKIN_ASHI_DIR_UP : HEIKIN_ASHI_DIR_DOWN);
  
  /* Event management
  if(dir != h->dir){
    h->dir = dir;
    if(dir == HEIKIN_ASHI_DIR_UP)
      indicator_set_event(i, c, HEIKIN_ASHI_EVENT_CHDIR_UP);
    else
      indicator_set_event(i, c, HEIKIN_ASHI_EVENT_CHDIR_DOWN);
      }*/
  
 out:
  return 1;
}

static void heikin_ashi_reset(struct indicator *i) {

  struct heikin_ashi *h = __indicator_self__(i);
  /* Nothing to do */
}

int heikin_ashi_init(struct heikin_ashi *h, indicator_id_t id) {
  
  /* Init parent */
  __indicator_super__(h, id, heikin_ashi_feed, heikin_ashi_reset);
  __indicator_set_string__(h, "heikin-ashi");

  /*
  h->value.close = (seed->open + seed->close + seed->high + seed->low) / 4;
  h->value.open = seed->open;
  h->value.high = seed->high;
  h->value.low = seed->low;
  */
  
  h->dir = HEIKIN_ASHI_DIR_NONE;
  return 0;
}

void heikin_ashi_release(struct heikin_ashi *h) {
  
  __indicator_release__(h);
}

int heikin_ashi_get(struct heikin_ashi *h, struct candle *candle) {
  
  memcpy(candle, &h->value, sizeof *candle);
  return 0;
}
