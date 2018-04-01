/*
 * Cresus EVO - yahoo_v7.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 01/02/2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef YAHOO_V7_H
#define YAHOO_V7_H

#include <stdio.h>
#include "framework/alloc.h"
#include "framework/input.h"

/* Object is allocatable */

#define yahoo_v7_alloc(ctx, filename)				\
  DEFINE_ALLOC(struct yahoo_v7, ctx, yahoo_v7_init, filename)
#define yahoo_v7_free(ctx)			\
  DEFINE_FREE(ctx, yahoo_v7_release)

struct yahoo_v7 {
  /* Inherits from input */
  __inherits_from_input__;
  /* file loader */
  FILE *fp;
  struct list *current_entry;
  /* Debug */
  char filename[256];
};

int yahoo_v7_init(struct yahoo_v7 *ctx, const char *filename);
void yahoo_v7_release(struct yahoo_v7 *ctx);

#endif
