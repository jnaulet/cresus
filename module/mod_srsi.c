/*
 * Cresus EVO - mod_srsi.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include "mod_srsi.h"

int mod_srsi_init(struct mod_srsi *m, const struct candle *seed) {
  
  module_init(&m->parent, 0.0, NULL, NULL);
  
  /* Add srsi */
  srsi_init(&m->srsi, 6000, seed);
  module_add_indicator(&m->parent, &m->srsi.parent);
  
  return 0;
}

void mod_srsi_free(struct mod_srsi *m) {
  
  /* Display report */
  for(int i = 0; i < m->srsi.max; i++)
    printf("%d : %d / %d (%d)\n", i, m->srsi.array[i].bull,
           m->srsi.array[i].bear, m->srsi.array[i].total);
  
  module_free(&m->parent);
  srsi_free(&m->srsi);
}
