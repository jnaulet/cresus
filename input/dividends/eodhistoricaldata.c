/*
 * Cresus - eodhistoricaldata.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/25/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "framework/verbose.h"
#include "framework/dividends.h"

struct eodhistoricaldata {
  FILE *fp;
};

static struct dividends_n3 *
eodhistoricaldata_parse_n3(struct eodhistoricaldata *ctx, char *str)
{
  struct dividends_n3 *n3;

  struct tm tm;
  time_t time = 0;
  double div = 0.0;

  /* Cut string */
  char *stime = strsep(&str, ",");
  char *sdiv = str; /* End */

  if(!stime || !sdiv)
    goto err;

  /* Set values */
  time = iso8601_to_time(stime);
  sscanf(sdiv, "%lf", &div);
  
  if(div != 0.0 && dividends_n3_alloc(n3, time, div))
    return n3;
  
 err:
  return NULL;
}

static struct dividends_n3 *
eodhistoricaldata_read(struct dividends *ctx)
{
  char buf[256];
  struct dividends_n3 *n3;
  struct eodhistoricaldata *e = ctx->private;

  while(fgets(buf, sizeof buf, e->fp)){
    /* Parse n3 */
    if((n3 = eodhistoricaldata_parse_n3(e, buf))){
      PR_DBG("%s %s loaded\n", ctx->filename, time_to_iso8601(n3->time));
      return n3;
    }
  }
  
  /* End of File */
  return NULL;
}

static int eodhistoricaldata_init(struct dividends *ctx)
{
  char dummy[256];
  
  struct eodhistoricaldata *e;
  if(!(e = calloc(1, sizeof(*e))))
    return -ENOMEM;

  /* open */
  if(!(e->fp = fopen(ctx->filename, "r")))
    goto err;
  
  /* Ignore first line */
  fgets(dummy, sizeof(dummy), e->fp);
  ctx->private = (void*)e;
  return 0;

 err:
  free(e);
  return -1;
}

static void eodhistoricaldata_release(struct dividends *ctx)
{
  struct eodhistoricaldata *e = ctx->private;
  if(e->fp) fclose(e->fp);
  free(e);
}

struct dividends_ops dividends_eodhistoricaldata_ops = {
  .init = eodhistoricaldata_init,
  .release = eodhistoricaldata_release,
  .read = eodhistoricaldata_read,
};
