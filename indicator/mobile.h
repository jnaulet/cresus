#ifndef MOBILE_H
#define MOBILE_H

/*
 * Mobile Average
 * Exponential & Simple
 *
 */

#include "math/average.h"
#include "framework/indicator.h"

typedef enum {
  MOBILE_MMA,
  MOBILE_EMA
} mobile_t;

typedef enum {
  MOBILE_DIR_UP,
  MOBILE_DIR_DOWN
} mobile_dir_t;

/* Warning
 * this is position of the mobile avg
 * compared to candle value
 * above means candle is below
 * and below means candle is above
 */
typedef enum {
  MOBILE_POS_ABOVE,
  MOBILE_POS_BELOW
} mobile_pos_t;

struct mobile {
  
  struct indicator parent;

  mobile_t type;
  mobile_dir_t dir;
  mobile_pos_t pos;

  /* Average object */
  struct average avg;
};

int mobile_init(struct mobile *m, mobile_t type, int period,
                candle_value_t value,
                const struct candle *seed);

void mobile_free(struct mobile *m);

int mobile_feed(struct indicator *i, const struct candle *candle);

/* indicator-specific */
double mobile_average(struct mobile *m);
double mobile_stddev(struct mobile *m);

#endif
