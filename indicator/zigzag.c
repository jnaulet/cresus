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

static int zigzag_feed(struct indicator *i, struct timeline_entry *e) {
  
  double threshold;
  struct zigzag *z = __indicator_self__(i);
  struct candle *candle = __timeline_entry_self__(e);

  if(!z->ref){
    z->ref = candle;
    return 0;
  }
  
  double ref_value = candle_get_value(z->ref, z->cvalue);
  double candle_value = candle_get_value(candle, z->cvalue);
  /* Now compute gap size */
  double gap = candle_value - ref_value;
  
  if(z->type == ZIGZAG_PERCENT)
    threshold = z->threshold * ref_value;
  else /* ZIGZAG_ABSOLUTE */
    threshold = z->threshold;

  z->ref_count++; /* FIXME */

  switch(z->direction){
  case ZIGZAG_DIR_UP :
    /* Up-ing reference */
    if(candle_value > ref_value){
      /* Reset ref */
      z->ref = candle;
      z->ref_count = 0;
    }
    
    /* Changing direction */
    if(gap <= -(threshold)){
      /* Save ref */
      z->last_ref = z->ref;
      z->ref = candle;
      z->ref_count = 0;
      /* Set dir */
      z->direction = ZIGZAG_DIR_DOWN;
      /* Throw event */
      //indicator_set_event(i, candle, ZIGZAG_EVENT_CHDIR_DOWN);
    }
    break;
    
  case ZIGZAG_DIR_DOWN :
    /* Down-ing reference */
    if(candle_value < ref_value){
      z->ref = candle;
      z->ref_count = 0;
    }
    
    /* Changing direction */
    if(gap >= threshold){
      /* Save ref */
      z->last_ref = z->ref;
      z->ref = candle;
      z->ref_count = 0;
      /* Set direction */
      /* Throw event */
      //indicator_set_event(i, candle, ZIGZAG_EVENT_CHDIR_UP);
    }
    break;
    
  case ZIGZAG_DIR_NONE :
    if(fabs(gap) >= threshold){
      z->direction = (gap > 0 ? ZIGZAG_DIR_UP : ZIGZAG_DIR_DOWN);
      /* Reset ref */
      z->ref = candle;
      z->ref_count = 0;
      /* Throw event */
      /*
      if(z->direction == ZIGZAG_DIR_UP)
	indicator_set_event(i, candle, ZIGZAG_EVENT_CHDIR_UP);
      else
	indicator_set_event(i, candle, ZIGZAG_EVENT_CHDIR_DOWN);
      */
    }
    break;
  }
  
  return z->direction;
}

int zigzag_init(struct zigzag *z, indicator_id_t id, zigzag_t type,
		double thres, candle_value_t cvalue) {
  
  /* Super */
  __indicator_super__(z, id, zigzag_feed);
  __indicator_set_string__(z, "zigzag[%.2f]", thres);
  
  z->type = type;
  z->cvalue = cvalue;
  z->threshold = (type == ZIGZAG_PERCENT ? thres / 100.0 : thres);

  /* Refs */
  /* z->ref = seed;
     z->last_ref = seed;*/
  z->ref = NULL;
  z->last_ref = NULL;
  z->ref_count = 0;
  
  z->direction = ZIGZAG_DIR_NONE;

  return 0;
}

void zigzag_release(struct zigzag *z)
{
  __indicator_release__(z);
  z->ref_count = 0;
  z->direction = ZIGZAG_DIR_NONE;
}

zigzag_dir_t zigzag_get_last_ref(struct zigzag *z, struct candle *last_ref)
{
  memcpy(last_ref, &z->last_ref, sizeof *last_ref);
  return z->direction;
}

