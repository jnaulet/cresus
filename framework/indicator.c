/*
 * Cresus EVO - indicator.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "indicator.h"

int indicator_init(struct indicator *i, indicator_feed_ptr feed) {
  
  /* Super */
  __slist_super__(i);
  timeline_entry_init(&i->list_entry, 0, 0);
  
  i->feed = feed;
  i->is_empty = 1;
  
  return 0;
}

void indicator_free(struct indicator *i) {

  __slist_free__(i);
  /* TODO : don't forget to free() timeline_entries */
  i->feed = NULL;
}

int indicator_feed(struct indicator *i, struct candle *c) {

  int ret = i->feed(i, c);
  i->is_empty = 0;

  return ret;
}

void indicator_set_event(struct indicator *i, struct candle *candle,
			 int event) {
  
  /* Nothing to do now */
  /* Set in candle or timeline maybe */
  __list_add_tail__(__list__(&i->list_entry),
		    __timeline_entry__(candle));
}
