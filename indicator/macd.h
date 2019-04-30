/*
 * Cresus EVO - macd.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef MACD_H
#define MACD_H

#include <stdlib.h>

#include "math/average.h"
#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

struct macd_n3 {
  /* As always */
  __inherits_from__(struct indicator_n3);
  /* Own data */
  double value;
  double signal;
  double histogram;
};

#define macd_n3_alloc(n3, parent, value, signal)		\
  DEFINE_ALLOC(struct macd_n3, n3,			\
	       macd_n3_init, parent, value, signal)
#define macd_n3_free(n3)			\
  DEFINE_FREE(n3, macd_n3_release)

static inline int macd_n3_init(struct macd_n3 *ctx,
				  struct indicator *parent,
				  double value, double signal)
{
  __indicator_n3_init__(ctx, parent);
  ctx->value = value;
  ctx->signal = signal;
  ctx->histogram = (value - signal);
  return 0;
}

static inline void macd_n3_release(struct macd_n3 *ctx)
{
  __indicator_n3_release__(ctx);
}

/* Indicator events */
#define MACD_EVENT_SIGNAL0 0 /* TODO */
#define MACD_EVENT_SIGNAL1 0 /* TODO */
#define MACD_EVENT_SIGNAL2 0 /* TODO */
#define MACD_EVENT_SIGNAL3 0 /* TODO */

/* Default params */
#define MACD_DEFAULT_FAST_P   12
#define MACD_DEFAULT_SLOW_P   26
#define MACD_DEFAULT_SIGNAL_P 9

#define macd_alloc(m, uid, fast_p, slow_p, signal_p)			\
  DEFINE_ALLOC(struct macd, m, macd_init, uid, fast_p, slow_p, signal_p)
#define macd_free(m)				\
  DEFINE_FREE(m, macd_release)

struct macd {
  /* Parent */
  __inherits_from__(struct indicator);
  /* 3 averages required */
  struct average fast;
  struct average slow;
  struct average signal;
};

int macd_init(struct macd *ctx, unique_id_t uid, int fast_p, int slow_p, int signal_p);
void macd_release(struct macd *ctx);

#endif
