/*
 * Cresus EVO - time64.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06.01.2018
 * Copyright (c) 2018 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "time64.h"

time64_t time64(void)
{
  return time64_epoch(time(NULL));
}

time64_t time64_epoch(time_t time)
{
  struct tm tm;
  time64_t t = 0;
  
  if(gmtime_r(&time, &tm))
    t = TIME64_INIT(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec, 0);
  
  return t;
}

time64_t time64_atot(char *str)
{
  /* Time info format : YYYY-MM-DD (date +%Y-%M-%d) */
  char *Y = strsep(&str, "-");
  char *M = strsep(&str, "-");
  char *d = str; /* End */
  return TIME64_INIT(atoi(Y), atoi(M), atoi(d), 0, 0, 0, 0);
}

int time64_dayofweek(time64_t t)
{
  static int x[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  int m = TIME64_GET_MONTH(t);
  int y = TIME64_GET_YEAR(t) - (m < 3);
  int d = TIME64_GET_DAY(t);
  return (y + y / 4 - y / 100 + y / 400 + x[m-1] + d) % 7;
}

const char *time64_str_r(time64_t t, time64_gr_t g, char *buf)
{
  *buf = 0;								
  char *ptr = buf;
  
  if(g & YEAR_MASK) ptr += sprintf(ptr, "%.4d-", TIME64_GET_YEAR(t));
  if(g & MONTH_MASK) ptr += sprintf(ptr, "%.2d-", TIME64_GET_MONTH(t));
  if(g & DAY_MASK) ptr += sprintf(ptr, "%.2d", TIME64_GET_DAY(t));
  if(g & HOUR_MASK) ptr += sprintf(ptr, " %.02d", TIME64_GET_HOUR(t));
  if(g & MINUTE_MASK) ptr += sprintf(ptr, ":%.02d", TIME64_GET_MINUTE(t));
  if(g & SECOND_MASK) ptr += sprintf(ptr, ":%.02d", TIME64_GET_SECOND(t));
  if(g & MSEC_MASK) ptr += sprintf(ptr, " :%.03d-", TIME64_GET_MSEC(t));
  
  return buf;
}

static char buf[256];

const char *time64_str(time64_t t, time64_gr_t g)
{
  return time64_str_r(t, g, buf);
}
