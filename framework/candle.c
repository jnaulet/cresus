#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "candle.h"

int candle_init(struct candle *c)
{
  memset(c, 0, sizeof *c);
  return 0;
}

void candle_free(struct candle *c)
{
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
  if(c->open > c->close) return -1;
  if(c->open < c->close) return 1;

  return 0;
}

time_t candle_localtime(const struct candle *c)
{
  return c->timestamp + MINUTES(c->offset);
}

int candle_localtime_match(struct candle *c, time_t min, time_t max)
{
  time_t time = candle_localtime(c);
  if(time >= min && time <= max)
    return 0;
  
  return -1;
}

int candle_daytime_match(struct candle *c, time_t min, time_t max)
{
  time_t time = candle_localtime(c);
  time -= DAY(time);

  if(time >= min && time <= max)
    return 0;

  return -1;
}

/* For debug purposes */
const char *candle_localtime_str(const struct candle *c, char *buf, size_t len)
{
  struct tm tm;
  time_t time = candle_localtime(c);

  strftime(buf, len, "%c", gmtime_r(&time, &tm));
  return buf;
}
