/*
 * Cresus EVO - yahoo.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef YAHOO_H
#define YAHOO_H

#include <stdio.h>
#include "framework/alloc.h"
#include "framework/input.h"

/* Object is allocatable */

#define yahoo_alloc(y, filename)			\
  DEFINE_ALLOC(struct yahoo, y, yahoo_init, filename)
#define yahoo_free(y)				\
  DEFINE_FREE(y, yahoo_release)

struct yahoo {
  /* Inherits from input */
  __inherits_from_input__;
  /* file loader */
  FILE *fp;
  /* Yahoo file format is LIFO */
  list_head_t(struct candle) list_entry;
  struct list *current_entry;
  /* Debug */
  char filename[256];
};

int yahoo_init(struct yahoo *y, const char *filename);
void yahoo_release(struct yahoo *y);

#endif
