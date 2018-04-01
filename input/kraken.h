/*
 * Cresus EVO - kraken.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 23/04/2019
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */
#ifndef KRAKEN_H
#define KRAKEN_H

#include <stdio.h>
#include <json-parser/json.h>

#include "framework/alloc.h"
#include "framework/input.h"

#define kraken_alloc(ctx, filename)				\
  DEFINE_ALLOC(struct kraken, ctx, kraken_init, filename)
#define kraken_free(ctx)			\
  DEFINE_FREE(ctx, kraken_release)

struct kraken {
  /* Inherits from input */
  __inherits_from_input__;
  /* internals */
  json_char *json;
  json_value *value;
  /* Our mix */
  int i;
  size_t len;
  json_value *data;
};

int kraken_init(struct kraken *ctx, const char *filename);
void kraken_release(struct kraken *ctx);

#endif
