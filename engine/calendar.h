/*
 * Cresus EVO - calendar.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 26/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef CALENDAR_H
#define CALENDAR_H

#include "framework/types.h"

struct calendar {
  time_info_t time;
  granularity_t g;
  /* Debug */
  char str[256];
};

int calendar_init(struct calendar *c, time_info_t time, granularity_t g);
void calendar_release(struct calendar *c);

int calendar_next(struct calendar *c, time_info_t *time);
const char *calendar_str(struct calendar *c);

static inline time_info_t calendar_time(struct calendar *c) {
  return c->time;
}

static inline granularity_t calendar_granularity(struct calendar *c) {
  return c->g;
}

#endif
