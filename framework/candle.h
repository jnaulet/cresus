#ifndef CANDLE_H
#define CANDLE_H

#include <time.h>
#include <float.h>

#define MINUTES(m) ((m) * 60)
#define HOURS(h) ((h) * 3600)
#define DAYS(d) ((d) * 86400)
#define DAY(d) ((d) - ((d) % DAYS(1)))

typedef enum {
  CANDLE_SOD,
  CANDLE_OTHER
} candle_t;

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
  
  time_t timestamp; /* UTC */
  int offset; /* In minutes (google mode) */
  
  /* Content */
  double open, close, high, low;
  double volume;

  /* Misc */
  candle_t type;
};

int candle_init(struct candle *c);
void candle_free(struct candle *c);

/* Is that useful ? */
double candle_get_closest_inf(struct candle *c, double value); /* TODO : Remove */
double candle_get_closest_sup(struct candle *c, double value); /* TODO : Remove */

double candle_get_value(const struct candle *c, candle_value_t value);
int candle_get_direction(const struct candle *c);

time_t candle_localtime(const struct candle *c);
const char *candle_localtime_str(const struct candle *c, char *buf, size_t len);

int candle_localtime_match(struct candle *c, time_t min, time_t max);
int candle_daytime_match(struct candle *c, time_t min, time_t max);

#endif
