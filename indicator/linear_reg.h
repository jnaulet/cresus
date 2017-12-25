/*
 * Cresus EVO - linear_reg.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 17/03/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef LINEAR_REG_H
#define LINEAR_REG_H

/*
 * Linear regression
 */

#include "math/average.h"
#include "engine/candle.h"

#include "framework/alloc.h"
#include "framework/indicator.h"

/* Timeline entries object */

struct linear_reg_entry {
  /* As below */
  __inherits_from_indicator_entry__;
  /* Single value */
  double value; /* End value for us */
  /* debug */
  double a, b; /* As in Y = aX + b */
};

#define linear_reg_entry_alloc(entry, parent, value)		\
  DEFINE_ALLOC(struct linear_reg_entry, entry,			\
	       linear_reg_entry_init, parent, value)
#define linear_reg_entry_free(entry)		\
  DEFINE_FREE(entry, linear_reg_entry_release)

static inline int linear_reg_entry_init(struct linear_reg_entry *entry,
					struct indicator *parent,
					double value)
{
  __indicator_entry_super__(entry, parent);
  entry->value = value;
  return 0;
}

static inline void linear_reg_entry_release(struct linear_reg_entry *entry)
{
  __indicator_entry_release__(entry);
}

/* Main object */

#define linear_reg_alloc(m, id, period)					\
  DEFINE_ALLOC(struct linear_reg, m, linear_reg_init, id, period)
#define linear_reg_free(m)			\
  DEFINE_FREE(m, linear_reg_release)

struct linear_reg {
  /* As always */
  __inherits_from_indicator__;
  /* Internals */
  int period;
};

int linear_reg_init(struct linear_reg *m, indicator_id_t id, int period);
void linear_reg_release(struct linear_reg *m);

#endif
