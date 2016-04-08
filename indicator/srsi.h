/*
 * Cresus EVO - srsi.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__srsi__
#define __Cresus_EVO__srsi__

#include "framework/indicator.h"

struct srsi {
  __inherits_from_indicator__;
  
  int max;
  int len;
  
  struct {
    int bull;
    int bear;
    int total;
  } *array;
};

int srsi_init(struct srsi *s, indicator_id_t id, int max);
void srsi_release(struct srsi *s);

#endif /* defined(__Cresus_EVO__srsi__) */
