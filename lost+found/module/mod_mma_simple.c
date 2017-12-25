/*
 * Cresus EVO - mod_mma_simple.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/21/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include "mod_mma_simple.h"

//#define STOCK 1300
#define CAPITAL 10000.0

static void mod_mma_simple_handle(struct module *m, const struct indicator *i,
                            event_t event, const struct candle *candle) {
  
  struct mod_mma_simple *mod = (struct mod_mma_simple*)m;

  /* Our personal events here */
  char buf[256];
  candle_localtime_str(candle, buf, sizeof buf);
  
  if((void*)i == (void*)&mod->mma){
    struct mobile *mma = (struct mobile*)i;
    if(event == EVENT_MOBILE_CROSSED_UP){
      if(mma->dir == MOBILE_DIR_UP){
        if(m->pshort.type == POSITION_SHORT){
          /* PUT trade pending : reverse */
          module_trade(m, MODULE_ACTION_EXITSHORT, CANDLE_OPEN, mod->stock); /* FIXME */
        }
        mod->stock = module_stock_from_capital(m, candle);
        module_trade(m, MODULE_ACTION_LONG, CANDLE_OPEN, mod->stock);
        stoploss_set(&mod->stoploss, STOPLOSS_DOWN, candle->open);
      }
    }
    if(event == EVENT_MOBILE_CROSSED_DOWN){
      if(mma->dir == MOBILE_DIR_DOWN){
        if(m->plong.type == POSITION_LONG){
          /* Call trade pending : revert */
          module_trade(m, MODULE_ACTION_SELL, CANDLE_OPEN, mod->stock);
        }
        mod->stock = module_stock_from_capital(m, candle);
        module_trade(m, MODULE_ACTION_SHORT, CANDLE_OPEN, mod->stock);
        stoploss_set(&mod->stoploss, STOPLOSS_UP, candle->open);
      }
    }
  }
  
  if((void*)i == (void*)&mod->stoploss){
    if(event == EVENT_STOPLOSS_HIT){
      printf("%s : stoploss hit\n", buf);
      if(m->plong.type == POSITION_LONG)
        module_trade(m, MODULE_ACTION_SELL, CANDLE_TYPICAL, mod->stock);
      if(m->pshort.type == POSITION_SHORT)
        module_trade(m, MODULE_ACTION_EXITSHORT, CANDLE_TYPICAL, mod->stock);
      
      stoploss_clear(&mod->stoploss);
    }
  }
}

int mod_mma_simple_init(struct mod_mma_simple *m, const struct candle *seed) {

  /* super */
  module_init(&m->parent, CAPITAL, mod_mma_simple_handle, NULL);
  
  /* Indicators */
  mobile_init(&m->mma, MOBILE_MMA, 30, CANDLE_CLOSE, seed); /* FIXME */
  stoploss_init(&m->stoploss, .5);
  
  /* Add */
  module_add_indicator(&m->parent, &m->mma.parent);
  module_add_indicator(&m->parent, &m->stoploss.parent);
  
  m->stock = 0;
  
  return 0;
}

void mod_mma_simple_release(struct mod_mma_simple *m) {
  
  mobile_release(&m->mma);
  module_release(&m->parent);
}
