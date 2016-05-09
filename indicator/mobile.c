/*
 * Cresus EVO - mobile.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mobile.h"

#if 0
static void mobile_manage_direction(struct mobile *m, double avg,
                                    struct candle *candle) {
  
  /* Check direction change */
  if(avg > m->avg.value){
    if(m->dir == MOBILE_DIR_DOWN)
      __indicator_set_event__(m, candle, MOBILE_EVENT_CHDIR_UP);
    
    m->dir = MOBILE_DIR_UP;
    
  }else if(avg < m->avg.value){
    if(m->dir == MOBILE_DIR_UP)
      __indicator_set_event__(m, candle, MOBILE_EVENT_CHDIR_DOWN);
    
    m->dir = MOBILE_DIR_DOWN;
  }
  
  /* If equals, keep last info */
}

static void mobile_manage_position(struct mobile *m, double avg,
                                   struct candle *candle) {
  
  double value = candle_get_value(candle, m->cvalue);
  if(avg > value){
    if(m->pos == MOBILE_POS_BELOW)
      __indicator_set_event__(m, candle, MOBILE_EVENT_CROSSED_DOWN);
    
    m->pos = MOBILE_POS_ABOVE;
    
  }else if(avg < value) {
    if(m->pos == MOBILE_POS_ABOVE)
      __indicator_set_event__(m, candle, MOBILE_EVENT_CROSSED_UP);
    
    m->pos = MOBILE_POS_BELOW;
  }
  
  /* If equals, do nothing */
}
#endif

static int mobile_feed(struct indicator *i, struct timeline_entry *e) {
  
  struct mobile_entry *entry;
  struct mobile *m = __indicator_self__(i);
  struct candle *c = __timeline_entry_self__(e);
  /* Trying to get average values */
  double last_avg = average_value(&m->avg);
  double avg = average_update(&m->avg, candle_get_value(c, m->cvalue));
  
  if(average_is_available(&m->avg)){
    /* Create new entry */
    if(mobile_entry_alloc(entry, i, avg, (avg - last_avg))){
      candle_add_indicator_entry(c, __indicator_entry__(entry));
      return 1;
    }
  }
  
  return 0;
}

static void mobile_reset(struct indicator *i) {

  struct mobile *m = __indicator_self__(i);
  average_reset(&m->avg);
}

int mobile_init(struct mobile *m, indicator_id_t id, mobile_t type,
		int period, candle_value_t cvalue) {
  
  /* Super */
  __indicator_super__(m, id, mobile_feed, mobile_reset);
  __indicator_set_string__(m, "%cma[%d]",
			   ((type == MOBILE_EMA) ? 'e' : 'm'),
			   period);

  m->type = type;
  m->cvalue = cvalue;

  switch(m->type) {
  case MOBILE_MMA : average_init(&m->avg, AVERAGE_MATH, period); break;
  case MOBILE_EMA : average_init(&m->avg, AVERAGE_EXP, period); break;
  }
  
  return 0;
}

void mobile_release(struct mobile *m)
{
  __indicator_release__(m);
  average_release(&m->avg);
}

double mobile_average(struct mobile *m)
{
  return average_value(&m->avg);
}

double mobile_stddev(struct mobile *m)
{
  return average_stddev(&m->avg);
}
