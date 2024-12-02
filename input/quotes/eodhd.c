/*
 * Cresus EVO - eodhd.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 01/02/2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "framework/quotes.h"
#include "framework/verbose.h"

struct eodhd {
  FILE *fp;
};

struct quotes_n3 *
eodhd_parse_n3(struct eodhd *ctx, char *str)
{
  struct quotes_n3 *n3;
  
  time_t time = 0;
  double open = 0.0, close = 0.0;
  double high = 0.0, low = 0.0;
  double volume = 0.0;
  
  /* Cut string */
  char *stime = strsep(&str, ",");
  char *sgmoff = strsep(&str, ",");
  char *sdatetime = strsep(&str, ",");
  char *sopen = strsep(&str, ",");
  char *shi = strsep(&str, ",");
  char *slo = strsep(&str, ",");
  char *sclose = strsep(&str, ",");
  char *svol = str; /* End */

  /* Check we got the right format */
  if(!stime || !sgmoff || !sdatetime ||
     !sopen || !shi || !slo || !sclose || !svol)
    goto err;
  
  /* Set values */
  time = iso8601_to_time(stime);
  sscanf(sopen, "%lf", &open);
  sscanf(sclose, "%lf", &close);
  sscanf(shi, "%lf", &high);
  sscanf(slo, "%lf", &low);
  sscanf(svol, "%lf", &volume);

  if(open != 0.0 && close != 0.0 && high != 0.0 && low != 0.0)
    if(quotes_n3_alloc(n3, time, open, close, high, low, volume))
      return n3;
  
 err:
  return NULL;
}

struct quotes_n3 *eodhd_read(struct quotes *ctx)
{
  char buf[256];
  struct quotes_n3 *n3;
  struct eodhd *y = ctx->private;
  
  while(fgets(buf, sizeof buf, y->fp)){
    /* Parse n3 */
    if((n3 = eodhd_parse_n3(y, buf))){
      PR_DBG("%s %s loaded\n", ctx->filename, time_to_iso8601(n3->time));
      return n3;
    }
  }
  
  /* End of File */
  return NULL;
}

int eodhd_init(struct quotes *ctx)
{
  char dummy[256];

  struct eodhd *y = calloc(1, sizeof(*y));
  if(!y) return -ENOMEM;
  
  if(!(y->fp = fopen(ctx->filename, "r")))
    goto err;
  
  /* Ignore first line */
  fgets(dummy, sizeof(dummy), y->fp);
  ctx->private = (void*)y;
  return 0;
  
 err:
  free(y);
  return -1;
}

void eodhd_release(struct quotes *ctx)
{
  struct eodhd *y = ctx->private;
  if(y->fp) fclose(y->fp);
  free(y);
}

struct quotes_ops eodhd_ops = {
  .init = eodhd_init,
  .release = eodhd_release,
  .read = eodhd_read,
};
