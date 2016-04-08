/*
 * Cresus EVO - mod_ema_ha.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 01/27/2015
 * Copyright (c) 2015 Joachim Naulet. All rights reserved.
 *
 */

#include "mod_ema_ha.h"

static void mod_ema_ha_handle(struct module *m, const struct indicator *i,
                              event_t event, const struct candle *candle) {

  struct mod_ema_ha *h = (struct mod_ema_ha*)m;
  
  if(event == EVENT_HEIKIN_ASHI_CHDIR){
    if(h->ha.dir == HEIKIN_ASHI_DIR_UP){
      if(h->mobile.dir == MOBILE_DIR_UP){
        /* Take position */
        module_trade(m, MODULE_ACTION_LONG, CANDLE_OPEN, 1);
      }
      /* Anyway */
      module_trade(m, MODULE_ACTION_EXITSHORT, CANDLE_OPEN, 1);
    }
    if(h->ha.dir == HEIKIN_ASHI_DIR_DOWN){
      if(h->mobile.dir == MOBILE_DIR_DOWN){
        /* Take position */
        module_trade(m, MODULE_ACTION_SHORT, CANDLE_OPEN, 1);
      }
      
      /* Anyway */
      module_trade(m, MODULE_ACTION_SELL, CANDLE_OPEN, 1);
    }
  }
}

static void mod_ema_ha_feed(struct module *m, const struct candle *candle) {
  
}

int mod_ema_ha_init(struct mod_ema_ha *e, const struct candle *seed) {
  
  module_init(&e->parent, 10000.0, mod_ema_ha_handle, NULL);
  
  /* Indicators */
  mobile_init(&e->mobile, MOBILE_EMA, 30, CANDLE_CLOSE, seed);
  heikin_ashi_init(&e->ha, seed);
  
  /* Add */
  module_add_indicator(&e->parent, &e->mobile.parent);
  module_add_indicator(&e->parent, &e->ha.parent);
  
  return 0;
}

void mod_ema_ha_release(struct mod_ema_ha *e) {
  
  module_release(&e->parent);
}
