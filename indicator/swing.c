/*
 * Cresus EVO - swing.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "swing.h"
#include <string.h>

static int swing_feed(struct indicator *i, struct candle *c) {
  
  struct swing *s = __indicator_self__(i);
  struct list *l = __list__(__timeline_entry__(c)); /* ancestors tree */

  /* TODO : back to original version
   * Problem with inside candles
   */
  if(!s->ref)
    goto out;
  
  if(c->high > s->ref->high || c->low < s->ref->low){
    /* Eliminate inside candles */
    if(++s->count >= SWING_MAX){
      /* Find 3 last candles */
      struct candle *s0, *s1, *s2;
      s2 = __timeline_entry_self__((struct timeline_entry*)__list_self__(l));
      s1 = __timeline_entry_self__((struct timeline_entry*)__list_self__(l->prev));
      s0 = __timeline_entry_self__((struct timeline_entry*)__list_self__(l->prev->prev));
      
      if(s0->low < s1->low && s2->low < s1->low &&
	 s0->high < s1->high && s2->high < s1->high)
	s->type = SWING_TOP;
      
      if(s0->low > s1->low && s2->low > s1->low &&
	 s0->high > s1->high && s2->high > s1->high)
	s->type = SWING_BOTTOM;
    }
  }

 out:
  s->ref = c;
  return 0;
}

int swing_init(struct swing *s) {
  
  /* Super() */
  __indicator_super__(s, swing_feed);
  __indicator_set_string__(s, "swing");
  
  s->count = 0;
  s->ref = NULL;
  s->type = SWING_NONE;

  /* swing_feed(__indicator__(s), seed); */
  return 0;
}

void swing_free(struct swing *s)
{
  __indicator_free__(s);
}

swing_t swing_type_get(struct swing *s)
{
  return s->type;
}
