/*
 * Cresus - dividends.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/25/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>

#include "framework/verbose.h"
#include "framework/dividends.h"

/*
 * Extern ops here
 */
extern struct dividends_ops dividends_eodhistoricaldata_ops;

/*
 * Private
 */

static struct dividends_ops *
dividends_ops_from_ext(struct dividends *ctx, const char *ext)
{
  return &dividends_eodhistoricaldata_ops;
}

static int dividends_load(struct dividends *ctx)
{
  /* Read dividends */
  struct dividends_n3 *n3;
  while((n3 = ctx->ops->read(ctx)) != NULL)
    SORT_BY_TIME(&ctx->list_dividends_n3s, n3, ctx->filename);
  
  return 0;
}

int dividends_init(struct dividends *ctx, const char *filename)
{
  int err;
  char *ext;

  list_head_init(&ctx->list_dividends_n3s);
  strncpy(ctx->filename, filename, sizeof ctx->filename);
  
  /* Wrapper */
  ext = strrchr(filename, '.') + 1;
  if(!(ctx->ops = dividends_ops_from_ext(ctx, ext))){
    PR_ERR("Unable to find dividends loader for extension .%s\n", ext);
    return -1;
  }
  
  if((err = ctx->ops->init(ctx)) < 0){
    PR_ERR("unable to load file %s (%d)\n", filename, err);
    return -1;
  }
  
  return dividends_load(ctx); /* FIXME */
}

void dividends_release(struct dividends *ctx)
{
  list_head_release(&ctx->list_dividends_n3s);
}
