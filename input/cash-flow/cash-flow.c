/*
 * Cresus Cash Flow - cash-flow.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 07/31/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "framework/verbose.h"
#include "framework/cash-flow.h"

/*
 * Extern ops here
 */
extern struct cash_flow_ops cash_flow_eodhistoricaldata_ops;

/*
 * Private
 */

static struct cash_flow_ops *
cash_flow_ops_from_ext(struct cash_flow *ctx, const char *ext)
{
  return &cash_flow_eodhistoricaldata_ops;
}

static int cash_flow_load(struct cash_flow *ctx)
{
  /* Read cash_flow */
  struct cash_flow_n3 *n3;
  while((n3 = ctx->ops->read(ctx)) != NULL)
    SORT_BY_TIME(&ctx->list_cash_flow_n3s, n3, ctx->filename);
  
  return 0;
}

/*
 * Public
 */

int cash_flow_init(struct cash_flow *ctx, const char *filename, period_t period)
{
  char *ext;
  int err = 0;
  
  ctx->private = NULL;
  ctx->period = period;
  list_head_init(&ctx->list_cash_flow_n3s);
  strncpy(ctx->filename, filename, sizeof ctx->filename);
  
  /* Wrapper */
  ext = strrchr(filename, '.') + 1;
  if(!(ctx->ops = cash_flow_ops_from_ext(ctx, ext))){
    PR_ERR("Unable to find cash_flow loader for extension .%s\n", ext);
    return -1;
  }
  
  if((err = ctx->ops->init(ctx)) < 0){
    PR_ERR("unable to load file %s (%d)\n", filename, err);
    return -1;
  }
  
  return cash_flow_load(ctx); /* FIXME */
}

void cash_flow_release(struct cash_flow *ctx)
{
  list_head_release(&ctx->list_cash_flow_n3s);
  ctx->ops->release(ctx);
}
