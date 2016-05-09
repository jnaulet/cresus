/*
 * Cresus EVO - types.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 13/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef TYPES_H
#define TYPES_H

/* MSEC   0-999   (1111100111) - 10 bits - << 0 */
/* SECOND 0-59        (111011) -  6 bits - << 10 */
/* MINUTE 0-59        (111011) -  6 bits - << 16 */
/* HOUR   0-23         (10111) -  5 bits - << 22 */
/* DAY    0-30         (11110) -  5 bits - << 27 */
/* MONTH  0-11          (1011) -  4 bits - << 32 */
/* YEAR   0-2047 (11111111111) - 11 bits - << 36 */

/* Limit is 12/31/2047 23:59:59 ::999 */

#define BITMASK(n) ((1ll << n) - 1)

#define MSEC_NBIT   10
#define SECOND_NBIT 6
#define MINUTE_NBIT 6
#define HOUR_NBIT   5
#define DAY_NBIT    5
#define MONTH_NBIT  4
#define YEAR_NBIT   11

#define MSEC_SHIFT   0
#define SECOND_SHIFT 10
#define MINUTE_SHIFT 16
#define HOUR_SHIFT   22
#define DAY_SHIFT    27
#define MONTH_SHIFT  32
#define YEAR_SHIFT   36

#define MSEC_MASK   (BITMASK(MSEC_NBIT)   << MSEC_SHIFT)
#define SECOND_MASK (BITMASK(SECOND_NBIT) << SECOND_SHIFT)
#define MINUTE_MASK (BITMASK(MINUTE_NBIT) << MINUTE_SHIFT)
#define HOUR_MASK   (BITMASK(HOUR_NBIT)   << HOUR_SHIFT)
#define DAY_MASK    (BITMASK(DAY_NBIT)    << DAY_SHIFT)
#define MONTH_MASK  (BITMASK(MONTH_NBIT)  << MONTH_SHIFT)
#define YEAR_MASK   (BITMASK(YEAR_NBIT)   << YEAR_SHIFT)

/* Some max info */
#define MSEC_MAX   999
#define SECOND_MAX 59
#define MINUTE_MAX 59
#define HOUR_MAX   23
#define DAY_MAX(t) /* TBD */
#define MONTH_MAX  11
#define YEAR_MAX   2047

/* Our basic types */

typedef long long time_info_t;
typedef long long granularity_t;

#define TIME_MIN  0
#define TIME_MAX -1

#define GRANULARITY_MSEC   (MSEC_MASK|SECOND_MASK|MINUTE_MASK|HOUR_MASK| \
			    DAY_MASK|MONTH_MASK|YEAR_MASK)
#define GRANULARITY_SECOND (SECOND_MASK|MINUTE_MASK|HOUR_MASK|	\
			    DAY_MASK|MONTH_MASK|YEAR_MASK)
#define GRANULARITY_MINUTE (MINUTE_MASK|HOUR_MASK|		\
			    DAY_MASK|MONTH_MASK|YEAR_MASK)
#define GRANULARITY_HOUR   (HOUR_MASK|DAY_MASK|MONTH_MASK|YEAR_MASK)
#define GRANULARITY_DAY    (DAY_MASK|MONTH_MASK|YEAR_MASK)
#define GRANULARITY_MONTH  (MONTH_MASK|YEAR_MASK)
#define GRANULARITY_YEAR   (YEAR_MASK)

/* Value generators */
#define VAL_MSEC(m) ((BITMASK(MSEC_NBIT) & m) << MSEC_SHIFT)
#define VAL_SECOND(s) ((BITMASK(SECOND_NBIT) & s) << SECOND_SHIFT)
#define VAL_MINUTE(m) ((BITMASK(MINUTE_NBIT) & m) << MINUTE_SHIFT)
#define VAL_HOUR(h) ((BITMASK(HOUR_NBIT) & h) << HOUR_SHIFT)
#define VAL_DAY(d) ((BITMASK(DAY_NBIT) & (d - 1)) << DAY_SHIFT)
#define VAL_MONTH(m) ((BITMASK(MONTH_NBIT) & (m - 1)) << MONTH_SHIFT)
#define VAL_YEAR(y) ((BITMASK(YEAR_NBIT) & y) << YEAR_SHIFT)

#define TIME_INIT(y, m, d, h, mn, s, ms)				\
  (VAL_MSEC(ms) | VAL_SECOND(s) | VAL_MINUTE(mn) | VAL_HOUR(h) |	\
   VAL_DAY(d) | VAL_MONTH(m) | VAL_YEAR(y))

/* Set accessors */
#define TIME_SET_MSEC(t, m)   t = ((t & ~MSEC_MASK)   | VAL_MSEC(m))
#define TIME_SET_SECOND(t, s) t = ((t & ~SECOND_MASK) | VAL_SECOND(s))
#define TIME_SET_MINUTE(t, m) t = ((t & ~MINUTE_MASK) | VAL_MINUTE(m))
#define TIME_SET_HOUR(t, h)   t = ((t & ~HOUR_MASK)   | VAL_HOUR(h))
#define TIME_SET_DAY(t, d)    t = ((t & ~DAY_MASK)    |	VAL_DAY(d))
#define TIME_SET_MONTH(t, m)  t = ((t & ~MONTH_MASK)  |	VAL_MONTH(m))
#define TIME_SET_YEAR(t, y)   t = ((t & ~YEAR_MASK)   | VAL_YEAR(y))

/* Get accessors */
#define TIME_GET_MSEC(t)   (int)((t & MSEC_MASK)    >> MSEC_SHIFT)
#define TIME_GET_SECOND(t) (int)((t & SECOND_MASK)  >> SECOND_SHIFT)
#define TIME_GET_MINUTE(t) (int)((t & MINUTE_MASK)  >> MINUTE_SHIFT)
#define TIME_GET_HOUR(t)   (int)((t & HOUR_MASK)    >> HOUR_SHIFT)
#define TIME_GET_DAY(t)    (int)(((t & DAY_MASK)    >> DAY_SHIFT) + 1)
#define TIME_GET_MONTH(t)  (int)(((t & MONTH_MASK)  >> MONTH_SHIFT) + 1)
#define TIME_GET_YEAR(t)   (int)((t & YEAR_MASK)    >> YEAR_SHIFT)

#define TIMECMP(t1, t2, g) ((t1 & g) - (t2 & g))
#define TIMEADD(t, g, unit) (t = (t + (GRANULARITY_MSEC & (~g + unit))))

/* Display */
static inline
const char *time2str(time_info_t t, granularity_t g, char *buf) {
  *buf = 0;								
  char *ptr = buf;							
  if(g & YEAR_MASK) ptr += sprintf(ptr, "%.4d-", TIME_GET_YEAR(t));
  if(g & MONTH_MASK) ptr += sprintf(ptr, "%.2d-", TIME_GET_MONTH(t));
  if(g & DAY_MASK) ptr += sprintf(ptr, "%.2d", TIME_GET_DAY(t));
  if(g & HOUR_MASK) ptr += sprintf(ptr, " %.02d", TIME_GET_HOUR(t));
  if(g & MINUTE_MASK) ptr += sprintf(ptr, ":%.02d", TIME_GET_MINUTE(t));
  if(g & SECOND_MASK) ptr += sprintf(ptr, ":%.02d", TIME_GET_SECOND(t));
  if(g & MSEC_MASK) ptr += sprintf(ptr, " :%.03d-", TIME_GET_MSEC(t));
  return buf;
}

/* Some kinda sync */
#define TIME_FOR_EACH(start, stop, g, time)		\
  for(time = start; time < stop; TIMEADD(time, g, 1))

/* Trend */

typedef enum {
  TREND_NONE,
  TREND_UP,
  TREND_DOWN
} trend_t;

#endif
