/*
 * Cresus EVO - input.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/21/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__input__
#define __Cresus_EVO__input__

#include <limits.h>
#include "timeline_entry.h"

#define __inherits_from_input__ struct input __parent_input__
#define __input_is_superclass__ void *__self_input__
#define __input__(x) (&(x)->__parent_input__)
#define __input_self__(x) (x)->__self_input__
#define __input_self_init__(x, self) __input_self__(x) = self

#define __input_super__(self, read, from, to)	\
  __input_self_init__(__input__(self), self);	\
  input_init(__input__(self), read, from, to)
#define __input_release__(self) input_release(__input__(self))

/* Methods */
#define __input_read__(self) input_read(__input__(self))

/* Typedefs */
struct input; /* FIXME : find another way */
typedef struct timeline_entry *(*input_read_ptr)(struct input *in);

struct input {
  __input_is_superclass__;
  input_read_ptr read;
  time_info_t from, to;
};

static inline int input_init(struct input *in, input_read_ptr read,
			     time_info_t from, time_info_t to)
{
  in->read = read;
  in->from = from;
  in->to = to;
  return 0;
}

static inline void input_release(struct input *in)
{
}

static inline struct timeline_entry *input_read(struct input *in)
{
  return in->read(in);
}

/* Shortcut macros */
#define input_in_boundaries(self, time, gr)			\
  (TIMECMP(time, (self)->from, gr) >= 0 &&			\
   TIMECMP(time, (self)->to, gr) <= 0)

#endif /* defined(__Cresus_EVO__input__) */
