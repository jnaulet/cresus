#ifndef ZIGZAG_H
#define ZIGZAG_H

#include "framework/candle.h"
#include "framework/indicator.h"

typedef enum {
   ZIGZAG_PERCENT,
   ZIGZAG_ABSOLUTE
} zigzag_t;

typedef enum {
  ZIGZAG_DIR_NONE,
  ZIGZAG_DIR_UP,
  ZIGZAG_DIR_DOWN
} zigzag_dir_t;

typedef enum {
  ZIGZAG_REF_HIGH,
  ZIGZAG_REF_LOW,
  ZIGZAG_REF_CLOSE,
  ZIGZAG_REF_OPEN,
  ZIGZAG_REF_TYPICAL
} zigzag_ref_t;

struct zigzag {
  
  struct indicator parent;

  zigzag_t type;
  double threshold;
  
  int ref_count; /* Candles since last ref */
  struct candle ref, last_ref;

  zigzag_dir_t direction; /* FIXME : use int ? */
};

int zigzag_init(struct zigzag *z, zigzag_t type, double thres,
                candle_value_t value, const struct candle *seed);

void zigzag_free(struct zigzag *z);

int zigzag_feed(struct indicator *i, const struct candle *candle);
zigzag_dir_t zigzag_get_last_ref(struct zigzag *z, struct candle *last_ref);

#endif
