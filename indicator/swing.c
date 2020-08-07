/*
 * Cresus EVO - swing.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "swing.h"
#include <string.h>

#define is_inside_candle(c, p)					\
  ((c)->quotes->high < (p)->quotes->high &&                                     \
   (c)->quotes->low > (p)->quotes->low)

static int swing_feed(struct indicator *i, struct track_n3 *e)
{  
  struct swing *ctx = (void*)i;
  struct candle *p, *c = (void*)e;
  struct candle *table[SWING_MAX];
  
  /* TODO : back to original version
   * Problem with inside candles
   */
  if(!ctx->ref)
    goto out;
  
  /* Populate ref table */
  for(int n = 0; n < SWING_MAX;){
    /* Beware of head if not enough data */
    if(__list_is_head__(c))
      goto out;

    /* Get last candle */
    p = __list_prev_n__(c, 1);
    
    if(!is_inside_candle(c, p))
      table[n++] = c;
    
    /* Navigate backwards */
    c = p;
  }
  
  /* Find 3 last candles */
  struct candle *s2 = table[0];
  struct candle *s1 = table[1];
  struct candle *s0 = table[2];
  
  if(s0->quotes->low < s1->quotes->low && s2->quotes->low < s1->quotes->low &&
     s0->quotes->high < s1->quotes->high && s2->quotes->high < s1->quotes->high)
    ctx->type = SWING_TOP;
  
  if(s0->quotes->low > s1->quotes->low && s2->quotes->low > s1->quotes->low &&
     s0->quotes->high > s1->quotes->high && s2->quotes->high > s1->quotes->high)
    ctx->type = SWING_BOTTOM;
  
 out:
  ctx->ref = c;
  return 0;
}

static void swing_reset(struct indicator *i)
{
  struct swing *ctx = (void*)i;
  /* RAZ */
  ctx->ref = NULL;
  ctx->type = SWING_NONE;
}

int swing_init(struct swing *ctx, unique_id_t id)
{  
  /* Super() */
  __indicator_init__(ctx, id, swing_feed, swing_reset);
  __indicator_set_string__(ctx, "swing");
  
  ctx->ref = NULL;
  ctx->type = SWING_NONE;

  /* swing_feed(__indicator__(s), seed); */
  return 0;
}

void swing_release(struct swing *ctx)
{
  __indicator_release__(ctx);
}

swing_t swing_type_get(struct swing *ctx)
{
  return ctx->type;
}
