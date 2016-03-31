//
//  average.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 20/11/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#ifndef __Cresus_EVO__average__
#define __Cresus_EVO__average__

typedef enum {
  AVERAGE_MATH,
  AVERAGE_EXP
} average_t;

struct average {
  average_t type;
  int period;
  /* Math */
  int count;
  int index;
  double *pool;
  /* Exponential */
  double k;
  /* All */
  double value;
};

int average_init(struct average *a, average_t type, int period, double seed);
void average_free(struct average *a);

double average_update(struct average *a, double value);

double average_value(struct average *a);
double average_stddev(struct average *a);

#endif /* defined(__Cresus_EVO__average__) */
