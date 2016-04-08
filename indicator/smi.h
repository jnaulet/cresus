/*
 * Cresus EVO - smi.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef SMI_H
#define SMI_H

/*
 * Stochastic Momentum Index
 */

#include "math/average.h"
#include "engine/candle.h"
#include "framework/indicator.h"

struct smi {
  /* Parent */
  __inherits_from_indicator__;

  /* Pool */
  int count;
  int index;
  int period;
  struct candle *pool;

  /* Computations */
  struct average smpd, _smpd;
  struct average str, _str;

  double value;
};

int smi_init(struct smi *s, indicator_id_t id, int period, int smooth);
void smi_release(struct smi *s);

/* Indicator-specific */
double smi_value(struct smi *s);

#endif
