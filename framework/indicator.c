/*
 * Cresus EVO - indicator.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "indicator.h"

int indicator_init(struct indicator *ctx,
                   unique_id_t uid,
		   indicator_feed_ptr feed,
                   indicator_reset_ptr reset)
{  
  /* Super */
  __slist_uid_init__(ctx, uid);
  
  /* Self */
  ctx->feed = feed;
  ctx->reset = reset;
  ctx->is_empty = 1;
  
  return 0;
}

void indicator_release(struct indicator *ctx)
{
  __slist_uid_release__(ctx);
  ctx->feed = NULL;
}

int indicator_feed(struct indicator *ctx, struct timeline_track_n3 *e)
{
  int ret = ctx->feed(ctx, e);
  ctx->is_empty = 0;
  
  return ret;
}

void indicator_reset(struct indicator *ctx)
{
  ctx->is_empty = 1;
  ctx->reset(ctx);
}
