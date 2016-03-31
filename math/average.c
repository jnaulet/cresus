//
//  average.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 20/11/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include <stdlib.h>
#include "average.h"

int average_init(struct average *a, average_t type, int period, double seed) {

  a->index = 1;
  a->count = 1;
  a->type = type;
  a->period = period;

  if(type == AVERAGE_MATH){
    /* Standard math average */
    if((a->pool = malloc(sizeof(*a->pool) * period)))
      return -1;
    
    a->pool[0] = seed;
    a->value = 0.0; /* FIXME : seed ? */
  }else{
    /* Exponential average */
    a->value = seed;
    a->k = 2.0 / (a->period + 1);
  }
  
  return 0;
}

void average_free(struct average *a) {
  
  if(a->type == AVERAGE_MATH)
    free(a->pool);
}

static double __average_update_math(struct average *a, double value) {
  
  double sum = 0.0;
  
  a->index = (a->index + 1) % a->period; /* Inc */
  a->pool[a->index] = value;
  a->count++;
  
  /* Compute simple average */
  if(a->count >= a->period){
    for(int i = a->period; i--;)
      sum += a->pool[i];

    a->value = sum / a->period;
    return a->value;
  }
  
  return 0.0;
}

static double __average_update_exp(struct average *a, double value) {

  a->value = a->k * value + (1 - a->k) * a->value;
  return a->value;
}

double average_update(struct average *a, double value) {
  
  switch(a->type){
  case AVERAGE_MATH : return __average_update_math(a, value);
  case AVERAGE_EXP : return __average_update_exp(a, value);
  }

  return 0.0;
}

double average_value(struct average *a) {

  return a->value;
}

double average_stddev(struct average *a) {

  double sum = 0.0;
  
  if(a->count >= a->period){
    for(int i = a->period; i--;)
      sum += pow(a->pool[i] - a->value, 2.0);
  }
  
  return sqrt(sum / a->period);
}
