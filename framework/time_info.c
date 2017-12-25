/*
 * Cresus EVO - time_info.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06.01.2018
 * Copyright (c) 2018 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include "time_info.h"

time_info_t time_info(void)
{
  return time_info_epoch(time(NULL));
}

time_info_t time_info_epoch(time_t time)
{
  struct tm tm;
  time_info_t t = 0;
  
  if(gmtime_r(&time, &tm))
    t = TIME_INIT(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		  tm.tm_hour, tm.tm_min, tm.tm_sec, 0);
  
  return t;
}

const char *time_info2str_r(time_info_t t, granularity_t g, char *buf)
{
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

static char buf[256];

const char *time_info2str(time_info_t t, granularity_t g)
{
  return time_info2str_r(t, g, buf);
}
