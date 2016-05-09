/*
 * Cresus EVO - zigzag.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <string.h>

#include "zigzag.h"

static void zigzag_chdir(struct zigzag *z, zigzag_dir_t dir,
			 struct candle *c) {
  
  z->base_ref = z->ref;
  z->dir = dir;  
  z->ref = c;
  z->ref_count = 0;
}

static int zigzag_feed(struct indicator *i, struct timeline_entry *e) {
  
  double threshold;
  struct zigzag_entry *zz;
  struct zigzag *z = __indicator_self__(i);
  struct candle *candle = __timeline_entry_self__(e);
  double value = candle_get_value(candle, z->cvalue);
  
  if(!z->ref){
    z->ref = candle;
    zigzag_chdir(z, ZIGZAG_NONE, candle);
    /* Nothing left to do */
    return 0;
  }
  
  /* Compute limits (every time ? */
  double base_ref_value = candle_get_value(z->base_ref, z->cvalue);
  double ref_value = candle_get_value(z->ref, z->cvalue);
  double hi_limit = (1.0 + z->threshold) * ref_value;
  double lo_limit = (1.0 - z->threshold) * ref_value;

  switch(z->dir){
  case ZIGZAG_NONE :
    if(value > hi_limit) zigzag_chdir(z, ZIGZAG_UP, candle);
    if(value < lo_limit) zigzag_chdir(z, ZIGZAG_DOWN, candle);
    break;

  case ZIGZAG_UP :
    if(value > ref_value) z->ref = candle;
    if(value < lo_limit) zigzag_chdir(z, ZIGZAG_DOWN, candle);
    break;
    
  case ZIGZAG_DOWN :
    if(value < ref_value) z->ref = candle;
    if(value > hi_limit) zigzag_chdir(z, ZIGZAG_UP, candle);
    break;
  }

  if(zigzag_entry_alloc(zz, i, z->dir, (value / base_ref_value), z->ref_count))
    candle_add_indicator_entry(candle, __indicator_entry__(zz));
  
  z->ref_count++;
  return 0;
}

static void zigzag_reset(struct indicator *i) {

  struct zigzag *z = __indicator_self__(i);
  /* RAZ */
  z->dir = ZIGZAG_NONE;
  /* Refs */
  z->ref = NULL;
  z->base_ref = NULL;
  z->ref_count = 0;
}

int zigzag_init(struct zigzag *z, indicator_id_t id,
		double threshold, candle_value_t cvalue) {
  
  /* Super */
  __indicator_super__(z, id, zigzag_feed, zigzag_reset);
  __indicator_set_string__(z, "zigzag[%.1f%%]", threshold * 100.0);

  z->dir = ZIGZAG_NONE;
  z->cvalue = cvalue;
  z->threshold = threshold;

  /* Refs */
  z->ref = NULL;
  z->base_ref = NULL;
  z->ref_count = 0;
  
  return 0;
}

void zigzag_release(struct zigzag *z)
{
  __indicator_release__(z);
  z->ref_count = 0;
  z->dir = ZIGZAG_NONE;
}
