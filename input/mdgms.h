/*
 * Cresus EVO - mdgms.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/01/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */
#ifndef MDGMS_H
#define MDGMS_H

#include <stdio.h>
#include <json-parser/json.h>

#include "framework/alloc.h"
#include "framework/input.h"

#define mdgms_alloc(ctx, filename)				\
  DEFINE_ALLOC(struct mdgms, ctx, mdgms_init, filename)
#define mdgms_free(ctx)				\
  DEFINE_FREE(ctx, mdgms_release)

struct mdgms {
  /* Inherits from input */
  __inherits_from_input__;
  /* internals */
  int i;
  json_char *json;
  json_value *value;
};

int mdgms_init(struct mdgms *ctx, const char *filename);
void mdgms_release(struct mdgms *ctx);

#endif
