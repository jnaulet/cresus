/*
 * Cresus - fundamentals.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/25/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "framework/time.h"
#include "framework/verbose.h"
#include "framework/fundamentals.h"

/*
 * Extern ops here
 */
extern struct fundamentals_ops fundamentals_eodhistoricaldata_ops;

/*
 * Private
 */

static struct fundamentals_ops *
fundamentals_ops_from_ext(struct fundamentals *ctx, const char *ext)
{
  return &fundamentals_eodhistoricaldata_ops;
}

#define LOAD(func, list, type)			\
  static int func(struct fundamentals *ctx){	\
  
  }

static int fundamentals_load(struct fundamentals *ctx)
{
  /* Read fundamentals */
  struct fundamentals_n3 *n3;
  while((n3 = ctx->ops->read(ctx)) != NULL)
    SORT_BY_TIME(&ctx->list_fundamentals_n3s, n3);
  
  return 0;
}

int fundamentals_init(struct fundamentals *ctx, const char *filename)
{
  char *ext;
  int err = 0;
  
  list_head_init(&ctx->list_fundamentals_n3s);
  strncpy(ctx->filename, filename, sizeof ctx->filename);
  
  /* Wrapper */
  ext = strrchr(filename, '.') + 1;
  if(!(ctx->ops = fundamentals_ops_from_ext(ctx, ext))){
    PR_ERR("Unable to find fundamentals loader for extension .%s\n", ext);
    return -1;
  }
  
  if((err = ctx->ops->init(ctx)) < 0){
    PR_ERR("unable to load file %s (%d)\n", filename, err);
    return -1;
  }
  
  return fundamentals_load(ctx); /* FIXME */
}

void fundamentals_release(struct fundamentals *ctx)
{
}
