/*
 * Cresus EVO - inwrap.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 01/01/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef INWRAP_H
#define INWRAP_H

#include "framework/alloc.h"
#include "framework/input.h"

#include "b4b.h"
#include "yahoo.h"
#include "yahoo_v7.h"
#include "json.h"

/* Object is allocatable */

#define inwrap_alloc(ctx, filename, type, from, to)			\
  DEFINE_ALLOC(struct inwrap, ctx, inwrap_init, filename,		\
	       type, from, to)
#define inwrap_free(ctx)			\
  DEFINE_FREE(ctx, inwrap_release)

typedef enum {
  INWRAP_YAHOO,
  INWRAP_YAHOO_V7,
  INWRAP_B4B,
  INWRAP_JSON,
  INWRAP_GOOGLE
} inwrap_t;

struct inwrap {
  /* Inherits from input */
  __inherits_from_input__;
  /* Suported types */  
  inwrap_t type;
  struct yahoo yahoo;
  struct yahoo_v7 yahoo_v7;
  struct b4b b4b;
  struct json json;
  // struct google google;
};

int inwrap_init(struct inwrap *ctx, const char *filename,
	       inwrap_t type, time_info_t from, time_info_t to);
void inwrap_release(struct inwrap *ctx);

#include <string.h>

static inline inwrap_t inwrap_t_from_str(const char *str)
{
  if(str){
    if(!strcmp("yahoo", str)) return INWRAP_YAHOO;
    if(!strcmp("b4b", str)) return INWRAP_B4B;
    if(!strcmp("google", str)) return INWRAP_GOOGLE;
    if(!strcmp("json", str) ||
       !strcmp("mdgms", str)) return INWRAP_JSON;
  }
  return INWRAP_YAHOO_V7;
}

#endif
