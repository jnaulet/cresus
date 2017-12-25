/*
 * Cresus EVO - inwrap.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 01/01/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>

#include "inwrap.h"
#include "engine/candle.h"
#include "framework/verbose.h"

static struct timeline_entry *inwrap_read(struct input *in)
{
  struct inwrap *ctx = __input_self__(in);
  switch(ctx->type){
  case INWRAP_YAHOO: return __input_read__(&ctx->yahoo);
  case INWRAP_YAHOO_V7: return __input_read__(&ctx->yahoo_v7);
  case INWRAP_B4B: return __input_read__(&ctx->b4b);
  case INWRAP_JSON: return __input_read__(&ctx->json);
  case INWRAP_GOOGLE: break;
  }

  return NULL;
}

int inwrap_init(struct inwrap *ctx, const char *filename,
	       inwrap_t type, time_info_t from, time_info_t to)
{
  int ret = -1;

  /* super() */
  __input_super__(ctx, inwrap_read, from, to);
  ctx->type = type;

  switch(type){
  case INWRAP_YAHOO:
    ret = yahoo_init(&ctx->yahoo, filename, from, to);
    break;

  case INWRAP_YAHOO_V7:
    ret = yahoo_v7_init(&ctx->yahoo_v7, filename, from, to);
    break;
    
  case INWRAP_B4B:
    ret = b4b_init(&ctx->b4b, filename, from, to);
    break;

  case INWRAP_JSON:
    ret = json_init(&ctx->json, filename, from, to);
    break;
    
  case INWRAP_GOOGLE:
    PR_WARN("INWRAP_GOOGLE not implemented\n");
    break;
  }
  
  return ret;
}

void inwrap_release(struct inwrap *ctx)
{
  __input_release__(ctx);
}

