/*
 * Cresus - splits.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/25/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>

#include "framework/verbose.h"
#include "framework/splits.h"

/*
 * Extern ops here
 */
extern struct splits_ops splits_eodhistoricaldata_ops;

/*
 * Private
 */

static struct splits_ops *
splits_ops_from_ext(struct splits *ctx, const char *ext)
{
  return &splits_eodhistoricaldata_ops;
}

static int splits_load(struct splits *ctx)
{
  /* Read splits */
  struct splits_n3 *n3;
  while((n3 = ctx->ops->read(ctx)) != NULL)
    SORT_BY_TIME(&ctx->list_splits_n3s, n3, ctx->filename);
  
  return 0;
}

int splits_init(struct splits *ctx, const char *filename)
{
  int err;
  char *ext;

  list_head_init(&ctx->list_splits_n3s);
  strncpy(ctx->filename, filename, sizeof ctx->filename);
  
  /* Wrapper */
  ext = strrchr(filename, '.') + 1;
  if(!(ctx->ops = splits_ops_from_ext(ctx, ext))){
    PR_ERR("Unable to find splits loader for extension .%s\n", ext);
    return -1;
  }
  
  if((err = ctx->ops->init(ctx)) < 0){
    PR_ERR("unable to load file %s (%d)\n", filename, err);
    return -1;
  }
  
  return splits_load(ctx); /* FIXME */
}

void splits_release(struct splits *ctx)
{
  list_head_release(&ctx->list_splits_n3s);
}
