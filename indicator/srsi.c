/*
 * Cresus EVO - srsi.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdlib.h>
#include "srsi.h"

static int srsi_feed(struct indicator *i, struct candle *c) {
  
  struct srsi *s = __indicator_self__(i);
  int start = (c->open < c->close ? c->open : c->close);
  int end = (c->close < c->open ? c->open : c->close);
  
  for(int i = start; i <= end; i++){
    if(c->close >= c->open)
      s->array[i].bull++;
    else
      s->array[i].bear++;
    
    s->array[i].total++;
  }
  
  s->len++;
  return 0;
}

int srsi_init(struct srsi *s, int max) {
  
  /* super() */
  __indicator_super__(s, srsi_feed);
  __indicator_set_string__(s, "srsi[%d]", max);
  
  s->len = 0;
  s->max = max;
  
  if((s->array = malloc(sizeof(*s->array) * max)))
    return -1;
  
  return 0;
}

void srsi_free(struct srsi *s) {
  
  __indicator_free__(s);
  free(s->array);
}
