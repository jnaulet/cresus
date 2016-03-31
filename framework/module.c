//
//  module.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 20/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include <stdio.h>
#include "module.h"

/* Module action */
static int module_action_init(struct module_action *m, module_action_t action,
                              candle_value_t value, int number) {
  
  m->action = action;
  m->value = value;
  m->number = number;
  
  return 0;
}

static void module_action_free(struct module_action *m) {
  
  m->action = MODULE_ACTION_NONE;
}

/* Module */
static void module_event_handle(struct event_handler *h,
                                const struct indicator *i, event_t event,
                                const struct candle *candle) {
  
  /* Forward event */
  struct module *m = (struct module*)h;
  m->handle(m, i, event, candle);
}

int module_init(struct module *m, double capital,
                module_event_handle_ptr handle,
                module_feed_ptr feed) {
  
  /* super */
  event_handler_init(&m->parent, module_event_handle);
  m->feed = feed;
  
  m->capital = capital;
  m->handle = handle;
  m->indicators = 0;
  m->actions = 0;
  
  trade_init(&m->trade, TRADE_NONE, NULL, 0, 0);
  position_init(&m->plong, POSITION_NONE, NULL, 0, 0);
  position_init(&m->pshort, POSITION_NONE, NULL, 0, 0);
  
  statistics_init(&m->statistics, 5.0); /* FIXME */
  
  return 0;
}

void module_free(struct module *m) {
  
  event_handler_free(&m->parent);
  
  m->indicators = 0;
  m->actions = 0;
  
  if(m->trade.type != TRADE_NONE){
    /* Do last trade */
    double result = trade_get_value(&m->trade);
    statistics_feed(&m->statistics, result);
  }
  trade_free(&m->trade);
  
  statistics_printf(&m->statistics);
  statistics_free(&m->statistics);
  
  /* Display capital */
  fprintf(stderr, "Final capital : %.2lf\n", m->capital);
}

int module_stock_from_capital(struct module *m, const struct candle *candle) {
  
  return 0.9 * (m->capital / candle->close);
}

int module_add_indicator(struct module *m, struct indicator *i) {
  
  if(m->indicators < MODULE_MAX_INDICATOR){
    m->indicator[m->indicators++] = i;
    indicator_register_event_handler(i, &m->parent);
    return 0;
  }
  
  return -1;
}

void module_feed(struct module *m, const struct candle *candle) {
  
  char buf[256];
  
  /* Feed current trade */
  trade_feed(&m->trade, candle);
  
  /*
  fprintf(stderr, "%s : %.2f [%.2f]\n",
          candle_localtime_str(candle, buf, sizeof buf),
          candle->close, candle->volume);
   */
  
  /* Is action pending */
  for(int i = 0; i < m->actions; i++){
    /* Stock value */
    double stock_value = m->action[i].number *
      candle_get_value(candle, m->action[i].value);
    
    switch(m->action[i].action){
      case MODULE_ACTION_LONG :
        if(m->plong.type == POSITION_NONE){
          position_init(&m->plong, POSITION_LONG, candle,
                        m->action[i].value, m->action[i].number);
          
          fprintf(stderr, "%s : LONG order at %.2f\n",
                  candle_localtime_str(candle, buf, sizeof buf),
                  candle_get_value(candle, m->action[i].value));
        }else{
          fprintf(stderr, "Error : LONG position already taken\n");
        }
        break;
        
      case MODULE_ACTION_SHORT :
        if(m->pshort.type == POSITION_NONE){
          position_init(&m->pshort, POSITION_SHORT, candle,
                        m->action[i].value, m->action[i].number);
          
          fprintf(stderr, "%s : SHORT order at %.2f\n",
                  candle_localtime_str(candle, buf, sizeof buf),
                  candle_get_value(candle, m->action[i].value));
        }else{
          fprintf(stderr, "Error : SHORT position already taken\n");
        }
        break;
        
      case MODULE_ACTION_SELL :
        if(m->plong.type == POSITION_LONG){
          double res = position_get_value(&m->plong, candle, m->action[i].value);
          statistics_feed(&m->statistics, res);
          position_free(&m->plong);
          m->capital += res;
          
          fprintf(stderr, "%s : SELL order at %.2f (%+.2lf)\n",
                  candle_localtime_str(candle, buf, sizeof buf),
                  candle_get_value(candle, m->action[i].value),
                  res);
          
        }else{
          fprintf(stderr, "Error : no LONG position taken\n");
        }
        break;
        
      case MODULE_ACTION_EXITSHORT :
      if(m->pshort.type == POSITION_SHORT){
          double res = position_get_value(&m->pshort, candle, m->action[i].value);
          statistics_feed(&m->statistics, res);
          position_free(&m->pshort);
          m->capital += res;
          
          fprintf(stderr, "%s : EXISHORT order at %.2f (%+.2lf)\n",
                  candle_localtime_str(candle, buf, sizeof buf),
                  candle_get_value(candle, m->action[i].value),
                  res);
        }else{
          fprintf(stderr, "Error : no SHORT position taken\n");
        }
        break;
        
      default :
        break;
    }
    
    /* Reset action */
    module_action_free(&m->action[i]);
  }
  
  /* Reset actions list */
  m->actions = 0;
  
  for(size_t i = m->indicators; i--;)
    if(indicator_feed(m->indicator[i], candle) < 0)
      fprintf(stderr, "module_feed : indicator %ld not fed\n", i);
  
  /* Propagation */
  if(m->feed)
    m->feed(m, candle);
}

void module_trade(struct module *m, module_action_t action,
                  candle_value_t value, int number) {
  
  if(m->actions < MODULE_MAX_ACTION)
    module_action_init(&m->action[m->actions++],
                       action, value, number);
}
