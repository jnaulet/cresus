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

#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/input.h"

/* Object is allocatable */

#define b4b_alloc(ctx, filename)			\
  DEFINE_ALLOC(struct b4b, ctx, b4b_init, filename)
#define b4b_free(ctx)				\
  DEFINE_FREE(ctx, b4b_release)

struct b4b {
  /* Inherits from input */
  __inherits_from__(struct input);
  /* file loader */
  FILE *fp;
  struct list *current_n3;
  /* Debug */
  char filename[256];
};

int b4b_init(struct b4b *ctx, const char *filename);
void b4b_release(struct b4b *ctx);

#endif
