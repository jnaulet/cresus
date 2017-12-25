/*
 * Cresus EVO - mod_supres.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/13/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mod_supres.h"

#define SUPRES_MAX 7000
#define SUPRES_STEP 10

static void printnchar(char c, int n) {
  
  for(int i = n; i--;)
    fprintf(stdout, "%c", c);
}

static void mod_supres_handle(struct module *m, const struct indicator *i,
                              event_t event, const struct candle *candle) {
  
  struct mod_supres *mod = (struct mod_supres*)m;
  struct zigzag *zz = (struct zigzag*)i;
  
  /* Zigzag event here */
  struct candle last_ref;
  zigzag_get_last_ref(zz, &last_ref);
  
  int index = (int)last_ref.close / SUPRES_STEP;
  mod->table[index]++;
}

static void mod_supres_feed(struct module *m, const struct candle *candle) {
  
  struct mod_supres *mod = (struct mod_supres*)m;
  
  int spread = candle->high - candle->low;
  int index = (int)candle->low;
  
  for(int i = 0; i < spread; i++)
    mod->table[index + i]++;
}

int mod_supres_init(struct mod_supres *m, const struct candle *seed){
  
  /* super() */
  module_init(&m->parent, 0.0, mod_supres_handle, /*mod_supres_feed*/ NULL);
  
  /* Indicators */
  zigzag_init(&m->zigzag, ZIGZAG_PERCENT, 1.0, CANDLE_CLOSE, seed);
  /* Add to self */
  module_add_indicator(&m->parent, &m->zigzag.parent);
  
  /* */
  if((m->table = malloc(sizeof(int) * SUPRES_MAX))){
    memset(m->table, 0, sizeof(int) * SUPRES_MAX);
    return 0;
  }
  
  return -1;
}

void mod_supres_release(struct mod_supres *m){
  
  module_release(&m->parent);
  zigzag_release(&m->zigzag);
  
  /* Print output */
  for(int i = 0; i < SUPRES_MAX; i++){
    if(m->table[i] > 1){
      fprintf(stdout, "%d : ", i * SUPRES_STEP);
      printnchar('=', m->table[i]);
      fprintf(stdout, " %d\n", m->table[i]);
    }
  }
}
