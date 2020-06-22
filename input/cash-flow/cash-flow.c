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

/*
 * Private
 */


static struct cash_flow_ops *
cash_flow_ops_from_ext(struct cash_flow *ctx, const char *ext)
{
  /* Fail */
  return NULL;
}

static int cash_flow_load(struct cash_flow *ctx)
{
  time64_t t;
  struct list *l;
  struct cash_flow_n3 *n3;

  /* Read cash_flow */
  while((n3 = ctx->ops->read(ctx)) != NULL){
    /* Check errors */
    struct cash_flow_n3 *p = (void*)ctx->list_cash_flow_n3s.prev;
    
    if(list_is_head(&p->list))
      goto next;
    
    t = TIME64CMP(n3->time, p->time, GR_DAY);
    if(!t){ /* item already exists */
      PR_WARN("cash_flow_load: new n3 (%s) already exists\n",
              time64_str(n3->time, GR_DAY));
      continue;
    }
    if(t < 0){ /* list is ahead, warn */
      PR_WARN("cash_flow_load: new n3 (%s) comes before last n3 (%s)\n",
              time64_str(n3->time, GR_DAY), time64_str(p->time, GR_DAY));
      continue;
    }
    
  next:
    /* Append */
    list_add_tail(&ctx->list_cash_flow_n3s, &n3->list);
    PR_DBG("cash_flow.c: new n3 at %s\n", time64_str(n3->time, GR_DAY));
  }
  
  return 0;
}

/*
 * Public
 */

int cash_flow_init(struct cash_flow *ctx, const char *filename, const char *ext)
{
  int err = 0;
  
  ctx->private = NULL;
  list_head_init(&ctx->list_cash_flow_n3s);
  strncpy(ctx->filename, filename, sizeof ctx->filename);
  
  /* Wrapper */
  if(!ext) ext = strrchr(filename, '.') + 1;
  strncpy(ctx->ext, ext, sizeof ctx->ext);
  
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
