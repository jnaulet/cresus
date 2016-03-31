//
//  event_handler.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 21/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#ifndef __Cresus_EVO__event_handler__
#define __Cresus_EVO__event_handler__

#include "indicator.h"
#include "candle.h"
#include "event.h"

/* Define easy types */

struct event_handler;
typedef void (*event_handler_handle_ptr)(struct event_handler*,
  const struct indicator*, event_t, const struct candle*);

struct event_handler {
  event_handler_handle_ptr handle;
};

int event_handler_init(struct event_handler *h, event_handler_handle_ptr handle);
void event_handler_free(struct event_handler *h);

void event_handler_handle(struct event_handler *h,
                          const struct indicator *i, event_t event,
                          const struct candle *candle);

#endif /* defined(__Cresus_EVO__event_handler__) */
