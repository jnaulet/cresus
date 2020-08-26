/*
 * Cresus INcome Statement - income-statement.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 07/30/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "framework/verbose.h"
#include "framework/income-statement.h"

/*
 * Extern ops here
 */
extern struct income_statement_ops income_statement_eodhistoricaldata_ops;

/*
 * Private
 */

static struct income_statement_ops *
income_statement_ops_from_ext(struct income_statement *ctx, const char *ext)
{
  return &income_statement_eodhistoricaldata_ops;
}

static int income_statement_load(struct income_statement *ctx)
{
  /* Read income_statement */
  struct income_statement_n3 *n3;
  while((n3 = ctx->ops->read(ctx)) != NULL)
    SORT_BY_TIME(&ctx->list_income_statement_n3s, n3, ctx->filename);
  
  return 0;
}

/*
 * Public
 */

int income_statement_init(struct income_statement *ctx,
                          const char *filename,
                          period_t period)
{
  char *ext;
  int err = 0;
  
  ctx->private = NULL;
  ctx->period = period;
  list_head_init(&ctx->list_income_statement_n3s);
  strncpy(ctx->filename, filename, sizeof ctx->filename);
  
  /* Wrapper */
  ext = strrchr(filename, '.') + 1;
  if(!(ctx->ops = income_statement_ops_from_ext(ctx, ext))){
    PR_ERR("Unable to find income_statement loader for extension .%s\n", ext);
    return -1;
  }
  
  if((err = ctx->ops->init(ctx)) < 0){
    PR_ERR("unable to load file %s (%d)\n", filename, err);
    return -1;
  }
  
  return income_statement_load(ctx); /* FIXME */
}

void income_statement_release(struct income_statement *ctx)
{
  list_head_release(&ctx->list_income_statement_n3s);
  ctx->ops->release(ctx);
}
