#ifndef SMI_H
#define SMI_H

/*
 * Stochastic Momentum Index
 */

#include "math/average.h"
#include "framework/candle.h"
#include "framework/indicator.h"

struct smi {
  /* Parent */
  struct indicator parent;

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

int smi_init(struct smi *s, int period, int smooth, const struct candle *c);
void smi_free(struct smi *s);

int smi_feed(struct indicator *i, const struct candle *c);

/* Indicator-specific */
double smi_value(struct smi *s);

#endif
