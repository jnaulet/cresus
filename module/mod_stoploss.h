/*
 * Cresus EVO - mod_stoploss.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/24/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__mod_stoploss__
#define __Cresus_EVO__mod_stoploss__

#include "module.h"
#include "mobile.h"
#include "stoploss.h"

struct mod_stoploss {
  struct module parent;
  
  struct mobile mobile;
  struct stoploss stoploss;
  
  int move;
};

int mod_stoploss_init(struct mod_stoploss *s, const struct candle *seed);
void mod_stoploss_free(struct mod_stoploss *s);

#endif /* defined(__Cresus_EVO__mod_stoploss__) */
