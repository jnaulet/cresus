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
#include "framework/verbose.h"

int candle_init(struct candle *c,
		time_info_t time, granularity_t g,
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

  /* Indicators */
  slist_head_init(&c->slist_indicator);
  
  return 0;
}

void candle_release(struct candle *c) {
  
  __timeline_entry_release__(c);
  slist_head_release(&c->slist_indicator);
}

void candle_add_indicator_entry(struct candle *c,
				struct indicator_entry *e) {

  __slist_insert__(&c->slist_indicator, e);
}

void candle_merge(struct candle *c, struct candle *c2) {

  c->open += c2->open;
  c->close += c2->close;
  c->high += c2->high;
  c->low += c2->low;
  c->volume += c2->volume;
}

double candle_get_value(struct candle *c, candle_value_t value) {
  
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
  PR_WARN("bad value requested (%d)", value);
  return 0.0;
}

int candle_get_direction(struct candle *c)
{
  /*
  if(c->open > c->close) return -1;
  if(c->open < c->close) return 1;
  */
  return (c->close - c->open);
}

struct indicator_entry *candle_find_indicator_entry(struct candle *c,
						    indicator_id_t id) {
  
  struct indicator_entry *entry;
  __slist_for_each__(&c->slist_indicator, entry){
    if(entry->indicator->id == id)
      return entry;
  }
  
  PR_WARN("can't find any indicator %d entry\n", id);
  return NULL;
}

/* Debug */

const char *candle_str(struct candle *c, char *buf) {

  char tmp[256];
  sprintf(buf, "%s o%.1f c%.1f h%.1f l%.1f v%.0f",
	  __timeline_entry_str__(c, tmp, sizeof tmp),
	  c->open, c->close, c->high, c->low, c->volume);
  
  return buf;
}
