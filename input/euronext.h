/*
 * Cresus EVO - euronext.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 24/07/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */
#ifndef EURONEXT_H
#define EURONEXT_H

#include <stdio.h>
#include <json-parser/json.h>

#include "framework/alloc.h"
#include "framework/input.h"

#define euronext_alloc(ctx, filename)				\
  DEFINE_ALLOC(struct euronext, ctx, euronext_init, filename)
#define euronext_free(ctx)			\
  DEFINE_FREE(ctx, euronext_release)

struct euronext {
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

int euronext_init(struct euronext *ctx, const char *filename);
void euronext_release(struct euronext *ctx);

#endif
