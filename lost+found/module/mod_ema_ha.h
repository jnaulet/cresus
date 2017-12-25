/*
 * Cresus EVO - mod_ema_ha.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 01/27/2015
 * Copyright (c) 2015 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__mod_ema_ha__
#define __Cresus_EVO__mod_ema_ha__

#include "module.h"
#include "mobile.h"
#include "heikin_ashi.h"

struct mod_ema_ha {
  struct module parent;
  
  struct mobile mobile;
  struct heikin_ashi ha;
};

int mod_ema_ha_init(struct mod_ema_ha *e, const struct candle *seed);
void mod_ema_ha_release(struct mod_ema_ha *e);


#endif /* defined(__Cresus_EVO__mod_ema_ha__) */
