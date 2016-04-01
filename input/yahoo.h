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
#include "framework/input.h"

/* TODO : find a way to clearly tell what objects this input returns */
#define __yahoo_timeline_entry_t__(x) (struct candle*)(x)

struct yahoo {
  /* Inherits from input */
  __inherits_from_input__;
  /* file loader */
  FILE *fp;
};

int yahoo_init(struct yahoo *y, const char *filename);
void yahoo_free(struct yahoo *y);

#endif
