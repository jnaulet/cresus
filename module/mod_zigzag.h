/*
 * Cresus EVO - mod_zigzag.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/03/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__mod_zigzag__
#define __Cresus_EVO__mod_zigzag__

#include "module.h"
#include "candle.h"
#include "zigzag.h"
#include "mobile.h"
#include "stoploss.h"

struct mod_zigzag {
  struct module parent;
  struct zigzag zigzag;
  struct stoploss stoploss;
  
  struct mobile mma_price;
  struct mobile mma_volume;
  
  int stock;
};

int mod_zigzag_init(struct mod_zigzag *m, const struct candle *seed);
void mod_zigzag_free(struct mod_zigzag *m);

#endif /* defined(__Cresus_EVO__mod_zigzag__) */
