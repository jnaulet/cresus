/*
 * Cresus EVO - mod_stoploss.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/24/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "mod_stoploss.h"

#define CAPITAL 10000.0

static void mod_stoploss_handle(struct module *m, const struct indicator *i,
                                event_t event, const struct candle *candle) {
  
  struct mod_stoploss *mod = (struct mod_stoploss*)m;
  
  switch(event){
    case EVENT_MOBILE_CROSSED_UP :
      if(mod->mobile.dir == MOBILE_DIR_UP &&
         m->trade.type == TRADE_NONE){
        int stock = module_stock_from_capital(m, candle);
        module_trade(m, MODULE_ACTION_LONG, CANDLE_TYPICAL, stock);
        stoploss_set(&mod->stoploss, STOPLOSS_DOWN, mod->mobile.avg);
        mod->move = 1;
      }
      break;
      
    case EVENT_MOBILE_CROSSED_DOWN :
      if(mod->mobile.dir == MOBILE_DIR_DOWN &&
         m->trade.type == TRADE_NONE){
        int stock = module_stock_from_capital(m, candle);
        module_trade(m, MODULE_ACTION_SHORT, CANDLE_TYPICAL, stock);
        stoploss_set(&mod->stoploss, STOPLOSS_UP, mod->mobile.avg);
        mod->move = 1;
      }
      break;
      
    case EVENT_STOPLOSS_HIT :
      module_trade(m, MODULE_ACTION_SELL, CANDLE_TYPICAL, m->trade.number);
      stoploss_clear(&mod->stoploss);
      mod->move = 0;
      break;
      
    default:
      break;
  }
}

void mod_stoploss_feed(struct module *m, const struct candle *candle) {
  
  struct mod_stoploss *mod = (struct mod_stoploss*)m;
  if(mod->move){ /* Useless */
    if(m->plong.type == POSITION_LONG){
      if(mod->mobile.avg > mod->stoploss.value)
        stoploss_set(&mod->stoploss, mod->stoploss.type, mod->mobile.avg);
    }
    if(m->pshort.type == POSITION_SHORT) {
      if(mod->mobile.avg < mod->stoploss.value)
        stoploss_set(&mod->stoploss, mod->stoploss.type, mod->mobile.avg);
    }
  }
}

int mod_stoploss_init(struct mod_stoploss *s, const struct candle *seed) {
  
  /* Super() */
  module_init(&s->parent, CAPITAL, mod_stoploss_handle, mod_stoploss_feed);
  
  /* Indicators */
  mobile_init(&s->mobile, MOBILE_MMA, 70, CANDLE_CLOSE, seed);
  stoploss_init(&s->stoploss, 0.0);
  
  module_add_indicator(&s->parent, &s->mobile.parent);
  module_add_indicator(&s->parent, &s->stoploss.parent);
  
  s->move = 0;
  
  return 0;
}

void mod_stoploss_free(struct mod_stoploss *s) {
  
  module_free(&s->parent);
  mobile_free(&s->mobile);
  stoploss_free(&s->stoploss);
}
