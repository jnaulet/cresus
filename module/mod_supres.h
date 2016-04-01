/*
 * Cresus EVO - mod_supres.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/13/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__mod_supres__
#define __Cresus_EVO__mod_supres__

#include "module.h"
#include "candle.h"
#include "zigzag.h"

struct mod_supres {
  struct module parent;
  struct zigzag zigzag;
  
  int *table;
};

int mod_supres_init(struct mod_supres *m, const struct candle *seed);
void mod_supres_free(struct mod_supres *m);

#endif /* defined(__Cresus_EVO__mod_supres__) */
