//
//  srsi.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 20/11/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include <stdlib.h>

#include "srsi.h"

int srsi_init(struct srsi *s, int max, const struct candle *seed) {
  
  /* super() */
  indicator_init(&s->parent, CANDLE_CLOSE, srsi_feed);
  
  s->len = 0;
  s->max = max;
  
  if((s->array = malloc(sizeof(*s->array) * max)))
    return -1;
  
  /* Init */
  srsi_feed(&s->parent, seed);
  return 0;
}

void srsi_free(struct srsi *s) {
  
  free(s->array);
}

int srsi_feed(struct indicator *i, const struct candle *candle) {
  
  struct srsi *s = (struct srsi*)i;
  int start = (candle->open < candle->close ? candle->open : candle->close);
  int end = (candle->close < candle->open ? candle->open : candle->close);
  
  for(int i = start; i <= end; i++){
    if(candle->close >= candle->open)
      s->array[i].bull++;
    else
      s->array[i].bear++;
    
    s->array[i].total++;
  }
  
  s->len++;
  return 0;
}
