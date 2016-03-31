//
//  mod_zigzag.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 23/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include <stdio.h>
#include "mod_zigzag.h"

#define CAPITAL 10000.0

static int attention = 0;

static double last_top_ref = 0.0;
static double last_bottom_ref = 0.0;

static void mod_zigzag_call(struct mod_zigzag *m,
                            const struct candle *candle) {
  
  m->stock = module_stock_from_capital(&m->parent, candle);
  module_trade(&m->parent, MODULE_ACTION_LONG, CANDLE_TYPICAL, m->stock);
  stoploss_set(&m->stoploss, STOPLOSS_DOWN, candle->open);
}

static void mod_zigzag_put(struct mod_zigzag *m,
                          const struct candle *candle) {
  
  m->stock = module_stock_from_capital(&m->parent, candle);
  module_trade(&m->parent, MODULE_ACTION_SHORT, CANDLE_TYPICAL, m->stock);
  stoploss_set(&m->stoploss, STOPLOSS_UP, candle->open);
}

static void mod_zigzag_handle_chdir_up(struct mod_zigzag *m,
                                       const struct candle *candle) {
  
  //if(m->mma_price.dir == MOBILE_DIR_UP &&
  //   candle->close > m->mma_price.avg){
    //if(candle->volume < (m->mma_volume.avg)){
  //if(attention > 0){

  /* No pending trade */
  mod_zigzag_call(m, candle);
  last_bottom_ref = candle->close;
  
  if(m->parent.pshort.type == POSITION_SHORT){
    module_trade(&m->parent, MODULE_ACTION_EXITSHORT, CANDLE_OPEN, m->stock);
    mod_zigzag_call(m, candle);
  }
    //}
    
  //  attention = 0;
  //}
}

static void mod_zigzag_handle_chdir_down(struct mod_zigzag *m,
                                         const struct candle *candle) {
  
  //if(m->mma_price.dir == MOBILE_DIR_DOWN &&
  //   candle->close < m->mma_price.avg){
    //if(candle->volume < (m->mma_volume.avg)){
  //if(attention < 0){
  
  /* No pending trade */
  mod_zigzag_put(m, candle);

  if(m->parent.plong.type == POSITION_LONG){
    module_trade(&m->parent, MODULE_ACTION_SELL, CANDLE_OPEN, m->stock);
    mod_zigzag_put(m, candle);
  }
    //}
    
    attention = 0;
  //}
}

void mod_zigzag_handle_volume_up(struct mod_zigzag *m,
                                 const struct candle *candle) {
  
  char date[256];
  candle_localtime_str(candle, date, sizeof date);
  
  //fprintf(stderr, "%s : Volume went over mobile average\n", date);
  if(candle->volume > m->mma_volume.avg * 1.5){
    attention = candle_get_direction(candle);
    fprintf(stderr, "%s : Volume went 1.5x the mobile average (%d)\n",
            date, attention);
  }
}

void mod_zigzag_handle_volume_down(struct mod_zigzag *m,
                                   const struct candle *candle) {
  
  char date[256];
  candle_localtime_str(candle, date, sizeof date);
  
  //fprintf(stderr, "%s : Volume went over mobile average\n", date);
  /*
  if(candle->volume > m->mma_volume.avg * .5)
    fprintf(stderr, "%s : Volume went below 0.5 the mobile average\n", date);
   */
}

static void mod_zigzag_handle(struct module *m, const struct indicator *i,
                              event_t event, const struct candle *candle) {
  
  char buf[256];
  struct mod_zigzag *mod = (struct mod_zigzag*)m;
  
  switch(event) {
    case EVENT_ZIGZAG_CHDIR :
      if(mod->zigzag.direction == ZIGZAG_DIR_UP)
        mod_zigzag_handle_chdir_up(mod, candle);
      if(mod->zigzag.direction == ZIGZAG_DIR_DOWN)
        mod_zigzag_handle_chdir_down(mod, candle);
      break;
      
    case EVENT_STOPLOSS_HIT :
      fprintf(stderr, "%s : stoploss hit\n", candle_localtime_str(candle, buf, sizeof buf));
      if(m->plong.type == POSITION_LONG)
        module_trade(m, MODULE_ACTION_SELL, CANDLE_TYPICAL, mod->stock);
      if(m->pshort.type == POSITION_SHORT)
        module_trade(m, MODULE_ACTION_EXITSHORT, CANDLE_TYPICAL, mod->stock);
      
      stoploss_clear(&mod->stoploss);
      break;
      
    case EVENT_MOBILE_CROSSED_UP :
      if((void*)i == (void*)&mod->mma_volume)
        mod_zigzag_handle_volume_up(mod, candle);
      break;
      
    case EVENT_MOBILE_CROSSED_DOWN :
      if((void*)i == (void*)&mod->mma_volume)
        mod_zigzag_handle_volume_down(mod, candle);
      break;
      
    default : break;
  }
}

int mod_zigzag_init(struct mod_zigzag *m, const struct candle *seed) {
  
  /* super() */
  module_init(&m->parent, CAPITAL, mod_zigzag_handle, NULL);
  
  /* Indicators */
  zigzag_init(&m->zigzag, ZIGZAG_PERCENT, 10.0, CANDLE_CLOSE, seed);
  stoploss_init(&m->stoploss, 3.0);
  
  mobile_init(&m->mma_price, MOBILE_MMA, 12, CANDLE_CLOSE, seed);
  mobile_init(&m->mma_volume, MOBILE_MMA, 40, CANDLE_VOLUME, seed);
  
  /* Add to self */
  module_add_indicator(&m->parent, &m->zigzag.parent);
  module_add_indicator(&m->parent, &m->stoploss.parent);
  
  module_add_indicator(&m->parent, &m->mma_price.parent);
  module_add_indicator(&m->parent, &m->mma_volume.parent);
  
  /* Content */
  m->stock = 0;
  
  return 0;
}

void mod_zigzag_free(struct mod_zigzag *m) {
  
  module_free(&m->parent);
  
  stoploss_free(&m->stoploss);
  mobile_free(&m->mma_price);
  mobile_free(&m->mma_volume);
  
  zigzag_free(&m->zigzag);
}
