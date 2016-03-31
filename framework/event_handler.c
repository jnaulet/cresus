//
//  event_handler.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 21/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include "event_handler.h"

int event_handler_init(struct event_handler *h,
                       event_handler_handle_ptr handle) {

  h->handle = handle;
  return 0;
}

void event_handler_free(struct event_handler *h) {
  
  h->handle = NULL;
}


void event_handler_handle(struct event_handler *h,
                          const struct indicator *i, event_t event,
                          const struct candle *candle) {
  
  h->handle(h, i, event, candle);
}
