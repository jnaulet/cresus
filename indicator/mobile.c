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

static int mobile_feed(struct indicator *i, struct candle *c) {
  
  struct mobile *m = __indicator_self__(i);
  average_update(&m->avg, candle_get_value(c, m->cvalue));
  
  /* Check direction change */
  mobile_manage_direction(m, m->avg.value, c);
  /* Check position change */
  mobile_manage_position(m, m->avg.value, c);
  
  return m->avg.value;
}

int mobile_init(struct mobile *m, mobile_t type,
		int period, candle_value_t cvalue) {
  
  /* Super */
  __indicator_super__(m, mobile_feed);
  __indicator_set_string__(m, "%cma[%d]",
			   ((type == MOBILE_EMA) ? 'e' : 'm'),
			   period);

  m->type = type;
  m->cvalue = cvalue;
  m->dir = MOBILE_DIR_UP; /* FIXME */
  m->pos = MOBILE_POS_BELOW;

  switch(m->type) {
  case MOBILE_MMA : average_init(&m->avg, AVERAGE_MATH, period); break;
  case MOBILE_EMA : average_init(&m->avg, AVERAGE_EXP, period); break;
  }
  
  return 0;
}

void mobile_free(struct mobile *m)
{
  __indicator_free__(m);
  average_free(&m->avg);
}

double mobile_average(struct mobile *m)
{
  return average_value(&m->avg);
}

double mobile_stddev(struct mobile *m)
{
  return average_stddev(&m->avg);
}
