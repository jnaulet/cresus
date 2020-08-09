/*
 * Cresus EVO - quotes.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 07/07/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "framework/quotes.h"
#include "framework/verbose.h"

/*
 * Extern ops here
 */
extern struct quotes_ops b4b_ops;
extern struct quotes_ops mdgms_ops;
extern struct quotes_ops xtrade_ops;
extern struct quotes_ops euronext_ops;
extern struct quotes_ops kraken_ops;
extern struct quotes_ops yahoo_v7_ops;

/*
 * Private
 */

static struct quotes_ops *
quotes_ops_from_ext(struct quotes *ctx, const char *ext)
{
  if(!strcmp("b4b", ext)) return &b4b_ops;
  else if(!strcmp("mdgms", ext)) return &mdgms_ops;
  else if(!strcmp("xtrade", ext)) return &xtrade_ops;
  else if(!strcmp("euronext", ext)) return &euronext_ops;
  else if(!strcmp("kraken", ext)) return &kraken_ops;
  /* Default yahoo v7 */
  else return &yahoo_v7_ops;
  
  /* Fail */
  return NULL;
}

static int quotes_load(struct quotes *ctx)
{
  time64_t t;
  struct list *l;
  struct quotes_n3 *n3;
  
  /* Read quotes */
  while((n3 = ctx->ops->read(ctx)) != NULL){
    /* Check errors */
    struct quotes_n3 *p = (void*)ctx->list_quotes_n3s.prev;

    if(list_is_head(&p->list))
      goto next;
    
    t = TIME64CMP(n3->time, p->time, GR_DAY);
    if(!t){ /* item already exists */
      PR_WARN("quotes_load: new n3 (%s) already exists\n",
              time64_str(n3->time, GR_DAY));
      continue;
    }
    if(t < 0){ /* list is ahead, warn */
      PR_WARN("quotes_load: new n3 (%s) comes before last n3 (%s)\n",
              time64_str(n3->time, GR_DAY), time64_str(p->time, GR_DAY));
      continue;
    }
    
  next:
    /* Append */
    list_add_tail(&ctx->list_quotes_n3s, &n3->list);
    PR_DBG("quotes.c: new n3 at %s\n", time64_str(n3->time, GR_DAY));
  }
  
  return 0;
}


/*
 * Public
 */

int quotes_init(struct quotes *ctx, const char *filename)
{
  char *ext;
  int err = 0;
  
  ctx->private = NULL;
  list_head_init(&ctx->list_quotes_n3s);
  strncpy(ctx->filename, filename, sizeof ctx->filename);
  
  /* Wrapper */
  ext = strrchr(filename, '.') + 1;
  if(!(ctx->ops = quotes_ops_from_ext(ctx, ext))){
    PR_ERR("Unable to find quotes loader for extension .%s\n", ext);
    return -1;
  }
  
  if((err = ctx->ops->init(ctx)) < 0){
    PR_ERR("unable to load file %s (%d)\n", filename, err);
    return -1;
  }
  
  return quotes_load(ctx); /* FIXME */
}

void quotes_release(struct quotes *ctx)
{
  list_head_release(&ctx->list_quotes_n3s);
  ctx->ops->release(ctx);
}
