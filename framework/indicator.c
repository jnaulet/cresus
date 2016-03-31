//
//  indicator.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 20/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include "indicator.h"

int indicator_init(struct indicator *i,
                   candle_value_t value,
                   indicator_feed_ptr feed) {
  
  i->value = value;
  i->feed = feed;
  return 0;
}

void indicator_free(struct indicator *i) {
  
  i->feed = NULL;
}

int indicator_feed(struct indicator *i, const struct candle *candle) {
  
  return i->feed(i, candle);
}

/*
void indicator_register_event_handler(struct indicator *i,
				      struct event_handler *h) {
  
  i->handler = h;
}

void indicator_throw_event(struct indicator *i, event_t event,
			   const struct candle *candle) {
  
  if(i->handler != NULL)
    event_handler_handle(i->handler, i, event, candle);
}
*/
