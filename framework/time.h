/*
 * Cresus - time.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/25/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef FRAMEWORK_TIME_H
#define FRAMEWORK_TIME_H

#include <time.h>
#include <stdio.h>
#include <limits.h>

#define TIME_MAX INT_MAX
#define timecmp(a, b) ((a) - (b))

/* ISO 8601 format */
static inline time_t iso8601_to_time(const char *str)
{
  int y, m, d;
  struct tm tm = { 0 };
  sscanf(str, "%d-%d-%d", &y, &m, &d);
  tm.tm_year = y - 1900;
  tm.tm_mon = m - 1;
  tm.tm_mday = d;
  return mktime(&tm);
}

static inline const char *time_to_iso8601(time_t time)
{
  static char str[256];
  struct tm *tm = localtime(&time);
  strftime(str, sizeof str, "%F", tm);
  return str;
}

/* Backwars compatibility */
#define time_atot(x) iso8601_to_time(x)
#define time_str(x, y) time_to_iso8601(x)

/* Sort any list entry by time */

#include "framework/list.h"
#include "framework/verbose.h"

#define SORT_BY_TIME(dlist, n3, name)					\
  do {									\
    typeof(n3) _p = (typeof(n3))list_prev_safe((dlist));		\
    if(!list_is_head(&_p->list)){					\
      time_t t = timecmp((n3)->time, _p->time);				\
      if(!t){								\
	PR_WARN("%s, %s: new n3 (%s) already exists\n",			\
		name, __FUNCTION__, time_to_iso8601((n3)->time));	\
	break;								\
      }									\
      if(t < 0){							\
	/* TODO: sort here */						\
	PR_WARN("%s, %s: new n3 (%s) comes before last n3 (%s)\n",	\
		name, __FUNCTION__, time_to_iso8601((n3)->time),	\
		time_to_iso8601(_p->time));				\
	break;								\
      }									\
    }									\
    /* Finally add to dlist */						\
    list_add_tail((dlist), &(n3)->list);				\
    PR_DBG("%s, %s: new n3 at %s\n", name, __FUNCTION__,		\
	   time_to_iso8601((n3)->time));				\
  } while(0);

#endif
