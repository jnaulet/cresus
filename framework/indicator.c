/*
 * Cresus EVO - indicator.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/20/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include "indicator.h"

int indicator_init(struct indicator *ctx,
                   unique_id_t uid,
		   indicator_feed_ptr feed,
                   indicator_reset_ptr reset)
{  
  ctx->uid = uid;
  ctx->feed = feed;
  ctx->reset = reset;
  ctx->is_empty = 1;
  
  return 0;
}

void indicator_release(struct indicator *ctx)
{
  ctx->feed = NULL;
}

int indicator_feed(struct indicator *ctx, struct track_n3 *e)
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
