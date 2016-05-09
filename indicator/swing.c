/*
 * Cresus EVO - swing.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "swing.h"
#include <string.h>

#define candle_prev(c) ((struct candle*)__timeline_entry_self__		\
			((struct timeline_entry*)__list_self__		\
			 (__list__(__timeline_entry__(c))->prev)))

#define is_inside_candle(c)					\
  ((c)->high < candle_prev(c)->high &&				\
   (c)->low > candle_prev(c)->low)

static int swing_feed(struct indicator *i, struct timeline_entry *e) {
  
  struct list *l = __list__(e);
  struct swing *s = __indicator_self__(i);
  struct candle *c = __timeline_entry_self__(e);
  
  struct candle *table[SWING_MAX];
  
  /* TODO : back to original version
   * Problem with inside candles
   */
  if(!s->ref)
    goto out;
  
  /* Populate ref table */
  for(int n = 0; n < SWING_MAX;){
    /* Beware of head if not enough data */
    if(__list__(__timeline_entry__(c)) == l->head)
      goto out;
    
    if(!is_inside_candle(c))
      table[n++] = c;
    
    /* Navigate backwards */
    c = candle_prev(c);
  }
  
  /* Find 3 last candles */
  struct candle *s2 = table[0];
  struct candle *s1 = table[1];
  struct candle *s0 = table[2];
  
  if(s0->low < s1->low && s2->low < s1->low &&
     s0->high < s1->high && s2->high < s1->high)
    s->type = SWING_TOP;
  
  if(s0->low > s1->low && s2->low > s1->low &&
     s0->high > s1->high && s2->high > s1->high)
    s->type = SWING_BOTTOM;
  
 out:
  s->ref = c;
  return 0;
}

static void swing_reset(struct indicator *i) {

  struct swing *s = __indicator_self__(i);
  /* RAZ */
  s->ref = NULL;
  s->type = SWING_NONE;
}

int swing_init(struct swing *s, indicator_id_t id) {
  
  /* Super() */
  __indicator_super__(s, id, swing_feed, swing_reset);
  __indicator_set_string__(s, "swing");
  
  s->ref = NULL;
  s->type = SWING_NONE;

  /* swing_feed(__indicator__(s), seed); */
  return 0;
}

void swing_release(struct swing *s)
{
  __indicator_release__(s);
}

swing_t swing_type_get(struct swing *s)
{
  return s->type;
}
