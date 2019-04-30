/*
 * Cresus EVO - google.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef GOOGLE_H
#define GOOGLE_H

#include <stdio.h>
#include <sys/types.h>

#include "framework/types.h"
#include "framework/input.h"

/* TODO : find a way to clearly tell what objects this input returns */
#define __google_timeline_n3_t__(x) (struct candle*)(x)

struct google {
  /* Super */
  __inherits_from__(struct input);
  /* Loader */
  FILE *fp;
  struct {
    int interval_s;
    int timezone_m;
    int time;
  } parser;
};

int google_init(struct google *g, const char *filename);
void google_release(struct google *g);

#endif
