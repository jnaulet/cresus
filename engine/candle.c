/*
 * Cresus EVO - candle.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "candle.h"

int candle_init(struct candle *c,
		time_t time, granularity_t g,
		double open, double close,
		double high, double low,
		double volume) {
  
  /* superclass */
  __timeline_entry_super__(c, time, g);
  
  /* Set */
  c->open = open;
  c->close = close;
  c->high = high;
  c->low = low;
  c->volume = volume;
  
  return 0;
}

void candle_free(struct candle *c) {
  
  __timeline_entry_free__(c);
}

struct candle *candle_alloc(time_t time, granularity_t g,
			    double open, double close,
			    double high, double low,
			    double volume) {

  struct candle *c = malloc(sizeof(struct candle));
  if(c) candle_init(c, time, g, open, close, high, low, volume);
  
  return c;
}

double candle_get_closest_inf(struct candle *c, double value)
{
  /* Chronologically seems more relevant */
  if(c->open <= value) return c->open;
  if(c->high <= value) return c->high; /* Not very useful */
  if(c->low <= value) return c->low;
  if(c->close <= value) return c->close;
  
  return 0.0;
}

double candle_get_closest_sup(struct candle *c, double value)
{
  /* Chronologically seems more relevant */
  if(c->open >= value) return c->open;
  if(c->low >= value) return c->low; /* Not very useful */
  if(c->high >= value) return c->high;
  if(c->close >= value) return c->close;

  return 0.0;
}

double candle_get_value(const struct candle *c, candle_value_t value) {
  
  switch(value) {
    case CANDLE_OPEN : return c->open;
    case CANDLE_CLOSE : return c->close;
    case CANDLE_HIGH : return c->high;
    case CANDLE_LOW : return c->low;
    case CANDLE_VOLUME : return c->volume;
    case CANDLE_TYPICAL : return (c->high + c->low + c->close) / 3.0;
    case CANDLE_WEIGHTED : return (c->high + c->low + 2 * c->close) / 4.0;
    case CANDLE_MEDIAN : return (c->high + c->low) / 2.0;
    case CANDLE_TOTAL : return (c->high + c->low + c->open + c->close) / 4.0;
  }
  
  /* Unknown */
  fprintf(stderr, "candle_get_info : bad value requested (%d)", value);
  return 0.0;
}

int candle_get_direction(const struct candle *c)
{
  /*
  if(c->open > c->close) return -1;
  if(c->open < c->close) return 1;
  */
  return (c->close - c->open);
}

/* Debug */

const char *candle_str(struct candle *c) {

  sprintf(c->str, "%u -> o%.1f c%.1f h%.1f l%.1f v%.0f",
	  __timeline_entry__(c)->time,
	  c->open, c->close, c->high, c->low, c->volume);
	  
  return c->str;
}
