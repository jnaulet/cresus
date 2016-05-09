/*
 * Cresus EVO - indicator.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "indicator.h"

int indicator_init(struct indicator *i, indicator_id_t id,
		   indicator_feed_ptr feed, indicator_reset_ptr reset) {
  
  /* Super */
  __slist_super__(i);
  list_head_init(&i->list_entry);
  
  i->id = id;
  i->feed = feed;
  i->reset = reset;
  i->is_empty = 1;
  
  return 0;
}

void indicator_release(struct indicator *i) {

  __slist_release__(i);
  list_head_release(&i->list_entry);
  
  /* TODO : don't forget to release() timeline_entries */
  i->feed = NULL;
}

int indicator_feed(struct indicator *i, struct timeline_entry *e) {

  int ret = i->feed(i, e);
  i->is_empty = 0;

  return ret;
}

void indicator_reset(struct indicator *i) {

  i->is_empty = 1;
  i->reset(i);
}
