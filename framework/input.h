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

#include "framework/alloc.h"
#include "framework/time64.h"

typedef enum {
  OPEN = 0, CLOSE = 1, HIGH = 2, LOW = 3, VOLUME = 4
} input_n3_value_t;

#define input_n3_interface                              \
  time64_t time;                                        \
  union {                                               \
    struct { double open, close, high, low, volume; };  \
    double __input_n3_value__[0];                       \
  }

#define input_n3_interface_copy(a, b)                   \
  ({(a)->time = (b)->time;                              \
    (a)->open = (b)->open;                              \
    (a)->close = (b)->close;                            \
    (a)->high = (b)->high;                              \
    (a)->low = (b)->low;                                \
    (a)->volume = (b)->volume;})

#define input_n3_interface_fmt                  \
  "o%.2f c%.2f h%.2f l%.2f v%.0f"
#define input_n3_interface_args(itf)                                    \
  (itf)->open, (itf)->close, (itf)->high, (itf)->low, (itf)->volume
#define input_n3_interface_str(itf, buf)     \
  sprintf(buf, input_n3_interface_fmt,       \
          input_n3_interface_args(itf))

#define input_n3_value(itf, value)              \
  ((itf)->__input_n3_value__[value])

/* Input object format */

struct input_n3 {
  /* TODO : inherits this or use interface ? */
  __implements__(input_n3_interface);
};

static inline int input_n3_init(struct input_n3 *ctx,
                                time64_t time,
                                double open, double close,
                                double high, double low,
                                double volume)
{
  ctx->time = time;
  ctx->open = open;
  ctx->close = close;
  ctx->high = high;
  ctx->low = low;
  ctx->volume = volume;
  return 0;
}

static inline void input_n3_release(struct input_n3 *ctx)
{
  /* To be defined */
}

#define input_n3_alloc(ctx, time, open, close, high, low, volume) \
  DEFINE_ALLOC(struct input_n3, ctx, input_n3_init,               \
	       time, open, close, high, low, volume)
#define input_n3_free(ctx)			\
  DEFINE_FREE(ctx, input_n3_release)

#define __input__(x) ((struct input*)(x))

/* Methods */
#define __input_init__(ctx, read) input_init(__input__(ctx), read)
#define __input_release__(ctx) input_release(__input__(ctx))

#define __input_read__(ctx) input_read(__input__(ctx))

/* Typedefs */
struct input;
typedef struct input_n3 *(*input_read_ptr)(struct input *ctx);

struct input {
  input_read_ptr read;
};

static inline int input_init(struct input *ctx, input_read_ptr read)
{
  ctx->read = read;
  return 0;
}

static inline void input_release(struct input *ctx)
{
}

static inline struct input_n3 *input_read(struct input *ctx)
{
  return ctx->read(ctx);
}

#endif /* defined(__Cresus_EVO__input__) */
