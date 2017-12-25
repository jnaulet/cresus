/*
 * Cresus EVO - json.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/01/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */
#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include <json-parser/json.h>

#include "framework/alloc.h"
#include "framework/input.h"

#define json_alloc(ctx, filename, from, to)			\
  DEFINE_ALLOC(struct json, ctx, json_init, filename, from, to)
#define json_free(ctx)				\
  DEFINE_FREE(ctx, json_release)

struct json {
  /* Inherits from input */
  __inherits_from_input__;
  /* internals */
  int i;
  json_char *json;
  json_value *value;
};

int json_init(struct json *ctx, const char *filename,
	      time_info_t from, time_info_t to);
void json_release(struct json *ctx);

#endif
