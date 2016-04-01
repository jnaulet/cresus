/*
 * Cresus EVO - module.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__module__
#define __Cresus_EVO__module__

#include "trading/statistics.h"
#include "trading/position.h"
#include "trading/trade.h"

#define MODULE_MAX_INDICATOR 10
#define MODULE_MAX_ACTION    2

/* Typedefs */
struct module;
typedef void (*module_event_handle_ptr)(struct module*, const struct indicator*,
              event_t event, const struct candle*);
typedef void (*module_feed_ptr)(struct module *, const struct candle*);

typedef enum {
  MODULE_ACTION_NONE,
  MODULE_ACTION_LONG,
  MODULE_ACTION_SHORT,
  MODULE_ACTION_SELL,
  MODULE_ACTION_EXITSHORT
} module_action_t;

/* Internal use only */
struct module_action {
  module_action_t action;
  candle_value_t value;
  int number;
};

struct module {
  struct event_handler parent;
  
  /* For children */
  module_event_handle_ptr handle;
  module_feed_ptr feed;
  
  struct indicator *indicator[MODULE_MAX_INDICATOR];
  size_t indicators;
  
  struct module_action action[MODULE_MAX_ACTION];
  size_t actions;

  /* FIXME : deprecated */
  struct trade trade;
  
  /* FIXME : Use lists */
  struct position plong;
  struct position pshort;
  
  struct statistics statistics;
  double capital;
  
  /* TODO : Add name */
};

int module_init(struct module *m, double capital,
                module_event_handle_ptr handle,
                module_feed_ptr feed);

void module_free(struct module *m);

int module_stock_from_capital(struct module *m, const struct candle *candle);
int module_add_indicator(struct module *m, struct indicator *i);
void module_feed(struct module *m, const struct candle *candle);

void module_trade(struct module *m, module_action_t action,
                  candle_value_t value, int number);

#endif /* defined(__Cresus_EVO__module__) */
