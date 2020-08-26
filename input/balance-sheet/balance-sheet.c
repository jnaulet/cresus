/*
 * Cresus Balance Sheet - balance-sheet.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 07/30/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "framework/verbose.h"
#include "framework/balance-sheet.h"

/*
 * Extern ops here
 */
extern struct balance_sheet_ops balance_sheet_eodhistoricaldata_ops;

/*
 * Private
 */

static struct balance_sheet_ops *
balance_sheet_ops_from_ext(struct balance_sheet *ctx, const char *ext)
{
  return &balance_sheet_eodhistoricaldata_ops;
}

static int balance_sheet_load(struct balance_sheet *ctx)
{  
  /* Read balance_sheet */
  struct balance_sheet_n3 *n3;
  while((n3 = ctx->ops->read(ctx)) != NULL)
    SORT_BY_TIME(&ctx->list_balance_sheet_n3s, n3, ctx->filename);
  
  return 0;
}

/*
 * Public
 */

int balance_sheet_init(struct balance_sheet *ctx,
                       const char *filename,
                       period_t period)
{
  char *ext;
  int err = 0;
  
  ctx->private = NULL;
  ctx->period = period;
  list_head_init(&ctx->list_balance_sheet_n3s);
  strncpy(ctx->filename, filename, sizeof ctx->filename);
  
  /* Wrapper */
  ext = strrchr(filename, '.') + 1;
  if(!(ctx->ops = balance_sheet_ops_from_ext(ctx, ext))){
    PR_ERR("Unable to find balance_sheet loader for extension .%s\n", ext);
    return -1;
  }
  
  if((err = ctx->ops->init(ctx)) < 0){
    PR_ERR("unable to load file %s (%d)\n", filename, err);
    return -1;
  }
  
  return balance_sheet_load(ctx); /* FIXME */
}

void balance_sheet_release(struct balance_sheet *ctx)
{
  list_head_release(&ctx->list_balance_sheet_n3s);
  ctx->ops->release(ctx);
}
