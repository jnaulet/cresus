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
  case INWRAP_YAHOO_V7: return __input_read__(&ctx->yahoo_v7);
  case INWRAP_B4B: return __input_read__(&ctx->b4b);
  case INWRAP_MDGMS: return __input_read__(&ctx->mdgms);
  case INWRAP_XTRADE: return __input_read__(&ctx->xtrade);
  case INWRAP_EURONEXT: return __input_read__(&ctx->euronext);
  case INWRAP_KRAKEN: return __input_read__(&ctx->kraken);
  }
  
  return NULL;
}

int inwrap_init(struct inwrap *ctx, const char *filename, inwrap_t type)
{
  int ret = -1;

  /* super() */
  __input_super__(ctx, inwrap_read);
  ctx->type = type;

  switch(type){
  case INWRAP_YAHOO_V7:
    ret = yahoo_v7_init(&ctx->yahoo_v7, filename);
    break;
    
  case INWRAP_B4B:
    ret = b4b_init(&ctx->b4b, filename);
    break;

  case INWRAP_MDGMS:
    ret = mdgms_init(&ctx->mdgms, filename);
    break;

  case INWRAP_XTRADE:
    ret = xtrade_init(&ctx->xtrade, filename);
    break;
    
  case INWRAP_EURONEXT:
    ret = euronext_init(&ctx->euronext, filename);
    break;

  case INWRAP_KRAKEN:
    ret = kraken_init(&ctx->kraken, filename);
    break;
  }
  
  return ret;
}

void inwrap_release(struct inwrap *ctx)
{
  __input_release__(ctx);
}

