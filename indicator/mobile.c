#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mobile.h"

int mobile_init(struct mobile *m, mobile_t type,
                int period, candle_value_t value,
                const struct candle *seed)
{
  /* Super */
  indicator_init(&m->parent, value, mobile_feed);

  m->type = type;
  m->dir = MOBILE_DIR_UP; /* FIXME */
  m->pos = MOBILE_POS_BELOW;

  switch(m->type) {
  case MOBILE_MMA :
    average_init(&m->avg, AVERAGE_MATH, period,
		 candle_get_value(seed, value));
    break;
  case MOBILE_EMA :
    average_init(&m->avg, AVERAGE_EXP, period,
		 candle_get_value(seed, value));
    break;
  }

  return 0;
}

void mobile_free(struct mobile *m)
{
  indicator_free(&m->parent);
  average_free(&m->avg);
}

static void mobile_manage_direction(struct mobile *m, double avg,
                                    const struct candle *candle) {
  
  /* Check direction change */
  if(avg > m->avg.value){
    /*
    if(m->dir == MOBILE_DIR_DOWN)
      indicator_throw_event(&m->parent, EVENT_MOBILE_CHDIR_UP, candle);
    */
    
    m->dir = MOBILE_DIR_UP;
    
  }else if(avg < m->avg.value){
    /*
    if(m->dir == MOBILE_DIR_UP)
      indicator_throw_event(&m->parent, EVENT_MOBILE_CHDIR_DOWN, candle);
    */
    
    m->dir = MOBILE_DIR_DOWN;
  }
  
  /* If equals, keep last info */
}

static void mobile_manage_position(struct mobile *m, double avg,
                                   const struct candle *candle) {
  
  double candle_value = candle_get_value(candle, m->parent.value);
  if(avg > candle_value){
    /*
    if(m->pos == MOBILE_POS_BELOW)
      indicator_throw_event(&m->parent, EVENT_MOBILE_CROSSED_DOWN, candle);
    */
    
    m->pos = MOBILE_POS_ABOVE;
    
  }else if(avg < candle_value) {
    /*
    if(m->pos == MOBILE_POS_ABOVE)
      indicator_throw_event(&m->parent, EVENT_MOBILE_CROSSED_UP, candle);
    */
    
    m->pos = MOBILE_POS_BELOW;
  }
  
  /* If equals, do nothing */
}

int mobile_feed(struct indicator *i, const struct candle *candle)
{
  struct mobile *m = (struct mobile*)i;
  average_update(&m->avg, candle_get_value(candle, i->value));
  
  /* Check direction change */
  mobile_manage_direction(m, m->avg.value, candle);
  /* Check position change */
  mobile_manage_position(m, m->avg.value, candle);
  
  return m->avg.value;
}

double mobile_average(struct mobile *m)
{
  return average_value(&m->avg);
}

double mobile_stddev(struct mobile *m)
{
  return average_stddev(&m->avg);
}
