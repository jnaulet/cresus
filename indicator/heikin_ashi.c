//
//  heikin_ashi.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 26/01/2015.
//  Copyright (c) 2015 Joachim Naulet. All rights reserved.
//

#include <string.h>
#include "heikin_ashi.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

int heikin_ashi_init(struct heikin_ashi *h, const struct candle *seed)
{
  /* Init parent */
  indicator_init(&h->parent, CANDLE_CLOSE, heikin_ashi_feed);
  
  h->value.close = (seed->open + seed->close + seed->high + seed->low) / 4;
  h->value.open = seed->open; // Arbitrary
  h->value.high = seed->high;
  h->value.low = seed->low;
  
  h->dir = HEIKIN_ASHI_DIR_NONE;
  return 0;
}

void heikin_ashi_free(struct heikin_ashi *h)
{
  indicator_free(&h->parent);
}

int heikin_ashi_feed(struct indicator *i, const struct candle *candle)
{
  struct heikin_ashi *h = (struct heikin_ashi*)i;
  
  h->value.open = (h->value.close + h->value.open) / 2;
  h->value.close = (candle->open + candle->close + candle->high + candle->low) / 4;
  h->value.high = candle->high;
  h->value.low = candle->low;
  
  /* Define new direction */
  heikin_ashi_dir_t dir = ((h->value.open - h->value.close) < 0 ?
                           HEIKIN_ASHI_DIR_UP : HEIKIN_ASHI_DIR_DOWN);

  /*
  if(dir != h->dir){
    h->dir = dir;
    indicator_throw_event(i, EVENT_HEIKIN_ASHI_CHDIR, candle);
  }
  */
  
  return 1;
}

int heikin_ashi_get(struct heikin_ashi *h, struct candle *candle)
{
  memcpy(candle, &h->value, sizeof *candle);
  return 0;
}
