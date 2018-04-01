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
#include "mdgms.h"
#include "xtrade.h"
#include "euronext.h"
#include "kraken.h"

/* Object is allocatable */

#define inwrap_alloc(ctx, filename, type)			\
  DEFINE_ALLOC(struct inwrap, ctx, inwrap_init, filename, type)
#define inwrap_free(ctx)		\
  DEFINE_FREE(ctx, inwrap_release)

typedef enum {
  INWRAP_YAHOO_V7,
  INWRAP_B4B,
  INWRAP_MDGMS,
  INWRAP_XTRADE,
  INWRAP_EURONEXT,
  INWRAP_KRAKEN
} inwrap_t;

struct inwrap {
  /* Inherits from input */
  __inherits_from_input__;
  /* Suported types */  
  inwrap_t type;
  struct yahoo_v7 yahoo_v7;
  struct b4b b4b;
  struct mdgms mdgms;
  struct xtrade xtrade;
  struct euronext euronext;
  struct kraken kraken;
};

int inwrap_init(struct inwrap *ctx, const char *filename, inwrap_t type);
void inwrap_release(struct inwrap *ctx);

#include <string.h>

static inline inwrap_t inwrap_t_from_str(const char *str)
{
  if(str){
    if(!strcmp("yahoo", str)) return INWRAP_YAHOO_V7;
    if(!strcmp("b4b", str)) return INWRAP_B4B;
    if(!strcmp("mdgms", str)) return INWRAP_MDGMS;
    if(!strcmp("xtrade", str)) return INWRAP_XTRADE;
    if(!strcmp("euronext", str)) return INWRAP_EURONEXT;
    if(!strcmp("kraken", str)) return INWRAP_KRAKEN;
  }
  return INWRAP_YAHOO_V7;
}

#endif
