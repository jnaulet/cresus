/*
 * Cresus EVO - candle.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef CANDLE_H
#define CANDLE_H

#include <float.h>
#include <stdlib.h>

#include "framework/slist.h"
#include "framework/indicator.h"
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

/* Object is allocatable */
#define candle_alloc(c, time, g, open, close, high, low, volume)	\
  DEFINE_ALLOC(struct candle, c, candle_init,				\
	       time, g, open, close, high, low, volume)
#define candle_free(c)				\
  DEFINE_FREE(c, candle_release)

struct candle {
  /* Inherits from timeline,
   * so we don't need time management */
  __inherits_from_timeline_entry__;
  
  /* Content */
  double open, close;
  double high, low;
  double volume;
  
  /* Indicators information ? */
  __slist_head__(struct indicator_entry) slist_indicator;
  
  /* Debug */
  char str[256];
};

int candle_init(struct candle *c,
		time_info_t time, granularity_t g,
		double open, double close,
		double high, double low,
		double volume);
void candle_release(struct candle *c);

void candle_add_indicator_entry(struct candle *c,
				struct indicator_entry *e);

void candle_merge(struct candle *c, struct candle *c2);
double candle_get_value(struct candle *c, candle_value_t value);
int candle_get_direction(struct candle *c);
struct indicator *candle_find_indicator(struct candle *c, indicator_id_t id);

/* Debug */
const char *candle_str(struct candle *c);

#endif
