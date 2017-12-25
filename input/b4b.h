/*
 * Cresus EVO - b4b.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 12/30/2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef B4B_H
#define B4B_H

#include <stdio.h>
#include "framework/alloc.h"
#include "framework/input.h"

/* Object is allocatable */

#define b4b_alloc(ctx, filename, from, to)			\
  DEFINE_ALLOC(struct b4b, ctx, b4b_init, filename, from, to)
#define b4b_free(ctx)				\
  DEFINE_FREE(ctx, b4b_release)

struct b4b {
  /* Inherits from input */
  __inherits_from_input__;
  /* file loader */
  FILE *fp;
  struct list *current_entry;
  /* Debug */
  char filename[256];
};

int b4b_init(struct b4b *ctx, const char *filename,
	     time_info_t from, time_info_t to);
void b4b_release(struct b4b *ctx);

#endif
