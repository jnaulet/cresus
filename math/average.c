/*
 * Cresus EVO - average.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdlib.h>
#include "average.h"

int average_init(struct average *a, average_t type, int period)
{  
  a->index = 0;
  a->count = 0;
  a->value = 0;
  a->type = type;
  a->period = period;

  if(type == AVERAGE_MATH){
    /* Standard math average */
    if((a->pool = malloc(sizeof(*a->pool) * period)))
      return -1;
    
    /* a->pool[0] = seed; */
    /* a->value = 0.0; */
  }else{
    /* Exponential average */
    /* a->value = seed; */
    a->k = 2.0 / (a->period + 1.0);
  }
  
  return 0;
}

void average_release(struct average *a)
{  
  if(a->type == AVERAGE_MATH)
    free(a->pool);
}

void average_reset(struct average *a)
{
  /* Reset math values */
  a->index = 0;
  a->count = 0;
  a->value = 0;
  /* And exp, too */
  a->k = 2.0 / (a->period + 1);
}

int average_is_available(struct average *a)
{
  if(a->type == AVERAGE_EXP)
    return (a->count > 0);

  if(a->type == AVERAGE_MATH)
    return (a->count >= a->period);

  return 0;
}

static double average_update_math(struct average *a, double value)
{  
  double sum = 0.0;
  
  a->pool[a->index] = value;
  a->index = (a->index + 1) % a->period; /* Inc */
  
  /* Compute simple average */
  if(average_is_available(a)){
    for(int i = a->period; i--;)
      sum += a->pool[i];

    a->value = sum / a->period;
    return a->value;
  }
  
  return 0.0;
}

static double average_update_exp(struct average *a, double value)
{
  if(average_is_available(a))
    a->value = a->k * value + (1.0 - a->k) * a->value;
  else
    a->value = value;
  
  return a->value;
}

double average_update(struct average *a, double value)
{
  double ret = 0.0;
  switch(a->type){
  case AVERAGE_MATH : ret = average_update_math(a, value); break;
  case AVERAGE_EXP : ret = average_update_exp(a, value); break;
  }

  /* ? 1 : 0's not needed but i find this easier to read */
  a->count += ((a->count < a->period) ? 1 : 0);
  return ret;
}

double average_value(struct average *a)
{
  return a->value;
}

double average_stddev(struct average *a)
{
  double sum = 0.0;
  
  if(average_is_available(a)){
    for(int i = a->period; i--;)
      sum += pow(a->pool[i] - a->value, 2.0);
  }
  
  return sqrt(sum / a->period);
}
