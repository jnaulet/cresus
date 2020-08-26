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

#include "framework/splits.h"
#include "framework/verbose.h"

struct eodhistoricaldata {
  FILE *fp;
};

static struct splits_n3 *
eodhistoricaldata_parse_n3(struct eodhistoricaldata *ctx, char *str)
{
  struct splits_n3 *n3;

  struct tm tm;
  time_t time = 0;
  double fact = 0.0;
  double denm = 0.0;

  /* Cut string */
  char *stime = strsep(&str, ",");
  char *sfact = strsep(&str, "/");
  char *sdenm = str; /* End */
  
  if(!stime || !sfact || !sdenm)
    goto err;

  /* Set values */
  time = iso8601_to_time(stime);
  sscanf(sfact, "%lf", &fact);
  sscanf(sdenm, "%lf", &denm);
  
  if(fact != 0.0 && denm != 0.0)
    if(splits_n3_alloc(n3, time, fact, denm))
      return n3;
  
 err:
  return NULL;
}

static struct splits_n3 *
eodhistoricaldata_read(struct splits *ctx)
{
  char buf[256];
  struct splits_n3 *n3;
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

static int eodhistoricaldata_init(struct splits *ctx)
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

static void eodhistoricaldata_release(struct splits *ctx)
{
  struct eodhistoricaldata *e = ctx->private;
  if(e->fp) fclose(e->fp);
  free(e);
}

struct splits_ops splits_eodhistoricaldata_ops = {
  .init = eodhistoricaldata_init,
  .release = eodhistoricaldata_release,
  .read = eodhistoricaldata_read,
};
