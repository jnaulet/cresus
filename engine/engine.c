/*
 * Cresus EVO - engine.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/21/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include "engine.h"

int engine_init(struct engine *e, struct input *in) {
  
  e->input = in;
  e->modules = 0;
  
  return 0;
}

void engine_release(struct engine *e) {
  
  e->input = NULL;
  e->modules = 0;
}

int engine_add_module(struct engine *e, struct module *m) {
  
  if(e->modules < ENGINE_MAX_MODULE){
    e->module[e->modules++] = m;
    return 0;
  }
  
  return -1;
}

void engine_run(struct engine *e, int n) {
  
  struct candle candle;
  
  while(input_read(e->input, &candle) && n--){
    for(size_t i = e->modules; i--;)
      module_feed(e->module[i], &candle);
  }
}
