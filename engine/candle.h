/*
 * Cresus EVO - candle.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef CANDLE_H
#define CANDLE_H

#include <time.h>
#include <float.h>

#include "framework/timeline_entry.h"

typedef enum {
  CANDLE_OPEN,
  CANDLE_CLOSE,
  CANDLE_HIGH,
  CANDLE_LOW,
  CANDLE_VOLUME,
  CANDLE_TYPICAL,
  CANDLE_WEIGHTED,
  CANDLE_MEDIAN,
  CANDLE_TOTAL
  /* Other info here */
} candle_value_t;

struct candle {
  /* Inherits from timeline,
   * so we don't need time management */
  __inherits_from_timeline_entry__;
  
  /* Content */
  double open, close;
  double high, low;
  double volume;
  
  /* Misc */
  int offset; /* In minutes (google mode) : remove ? */

  /* Debug */
  char str[256];
};


int candle_init(struct candle *c,
		time_t time, granularity_t g,
		double open, double close,
		double high, double low,
		double volume);
void candle_free(struct candle *c);

struct candle *candle_alloc(time_t time, granularity_t g,
			    double open, double close,
			    double high, double low,
			    double volume);

/* Is that useful ? TODO : Remove these 2 */
double candle_get_closest_inf(struct candle *c, double value);
double candle_get_closest_sup(struct candle *c, double value);

double candle_get_value(const struct candle *c, candle_value_t value);
int candle_get_direction(const struct candle *c);

/* Debug */
const char *candle_str(struct candle *c);

#endif
