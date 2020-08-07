/*
 * Cresus EVO - yahoo_v7.c 
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

struct yahoo_v7 {
  FILE *fp;
};

struct quotes_n3 *
yahoo_v7_parse_n3(struct yahoo_v7 *ctx, char *str)
{
  struct quotes_n3 *n3;

  time64_t time = 0;
  int year, month, day;
  double open = 0.0, close = 0.0;
  double high = 0.0, low = 0.0;
  double volume = 0.0;
  
  /* Cut string */
  char *stime = strsep(&str, ",");
  char *sopen = strsep(&str, ",");
  char *shi = strsep(&str, ",");
  char *slo = strsep(&str, ",");
  char *sclose = strsep(&str, ",");
  char *adjclose = strsep(&str, ",");
  char *svol = str; /* End */

  /* Check we got the right format */
  if(!stime || !sopen || !shi || !slo ||
     !sclose || !adjclose || !svol)
    goto err;
  
  /* Set values */
  sscanf(stime, "%d-%d-%d", &year, &month, &day);
  sscanf(sopen, "%lf", &open);
  sscanf(sclose, "%lf", &close);
  sscanf(shi, "%lf", &high);
  sscanf(slo, "%lf", &low);
  sscanf(svol, "%lf", &volume);

  /* Dummy values for control */
  TIME64_SET_SECOND(time, 1);
  TIME64_SET_MINUTE(time, 30);
  TIME64_SET_HOUR(time, 17);
  
  TIME64_SET_DAY(time, day);
  TIME64_SET_MONTH(time, month);
  TIME64_SET_YEAR(time, year);

  if(open != 0.0 && close != 0.0 && high != 0.0 && low != 0.0)
    if(quotes_n3_alloc(n3, time, open, close, high, low, volume))
      return n3;
  
 err:
  return NULL;
}

struct quotes_n3 *yahoo_v7_read(struct quotes *ctx)
{
  char buf[256];
  struct quotes_n3 *n3;
  struct yahoo_v7 *y = ctx->private;
  
  while(fgets(buf, sizeof buf, y->fp)){
    /* Parse n3 */
    if((n3 = yahoo_v7_parse_n3(y, buf))){
      PR_DBG("%s %s loaded\n", ctx->filename,
             time64_str_r(n3->time, GR_DAY, buf));
      /* We got a new candle */
      return n3;
    }
  }
  
  /* End of File */
  return NULL;
}

int yahoo_v7_init(struct quotes *ctx)
{
  char dummy[256];

  struct yahoo_v7 *y = calloc(1, sizeof(*y));
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

void yahoo_v7_release(struct quotes *ctx)
{
  struct yahoo_v7 *y = ctx->private;
  if(y->fp) fclose(y->fp);
  free(y);
}

struct quotes_ops yahoo_v7_ops = {
  .init = yahoo_v7_init,
  .release = yahoo_v7_release,
  .read = yahoo_v7_read,
};
