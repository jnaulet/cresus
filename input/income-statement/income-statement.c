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
  time64_t t;
  struct list *l;
  struct income_statement_n3 *n3;

  /* Read income_statement */
  while((n3 = ctx->ops->read(ctx)) != NULL){
    /* Check errors */
    struct income_statement_n3 *p = (void*)ctx->list_income_statement_n3s.prev;
    
    if(list_is_head(&p->list))
      goto next;
    
    t = TIME64CMP(n3->time, p->time, GR_DAY);
    if(!t){ /* item already exists */
      PR_WARN("income_statement_load: new n3 (%s) already exists\n",
              time64_str(n3->time, GR_DAY));
      continue;
    }
    if(t < 0){ /* list is ahead, warn */
      PR_WARN("income_statement_load: new n3 (%s) comes before last n3 (%s)\n",
              time64_str(n3->time, GR_DAY), time64_str(p->time, GR_DAY));
      continue;
    }
    
  next:
    /* Append */
    list_add_tail(&ctx->list_income_statement_n3s, &n3->list);
    PR_DBG("income_statement.c: new n3 at %s\n", time64_str(n3->time, GR_DAY));
  }
  
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
