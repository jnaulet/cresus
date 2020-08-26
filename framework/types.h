/*
 * Cresus EVO - types.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 13/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef TYPES_H
#define TYPES_H

#define BITMASK(n) ((1ll << n) - 1)
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef unsigned int unique_id_t;
typedef enum {
  QUARTERLY,
  YEARLY,
  /* Max */
  PERIOD_MAX
} period_t;

/* FIXME: put somewhere else */
#define data(a, b)							\
  ({ typeof (a) _p_ = (a); while(_p_->b == 0.0){			\
      typeof(a) _q_ = (void*)list_prev_null(&_p_->list);		\
      if(_q_) _p_ = _q_; else break; } _p_->b; })

/* Exceptions */
#define __try__(cond, label) if(cond){ goto label; }
#define __catch__(label) label

#endif
