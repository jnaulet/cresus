/*
 * Cresus EVO - b4b.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 12/30/2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "framework/quotes.h"
#include "framework/verbose.h"

struct b4b {
  FILE *fp;
};

static ssize_t b4b_prepare_str(struct b4b *ctx, char *buf)
{
  int i;
  char *str = buf;
  
  for(i = 0; buf[i]; i++){
    /* Ignore parenthesis & spaces */
    if(buf[i] == '\"' || buf[i] == ' ')
      continue;
    
    /* Change commas to points */
    if(buf[i] == ',') *str++ = '.';
    else *str++ = buf[i];
  }

  /* Filter empty lines */
  if(*buf != '\r' && *buf != '\n'){
    PR_DBG("%s", buf);
    return i;
  }

  return -1;
}

static struct quotes_n3 *
b4b_parse_n3(struct b4b *ctx, char *str)
{
  time64_t time = 0;
  int year, month, day;
  struct quotes_n3 *n3;
  double open, close, high, low, volume; 
  
  /* Cut string */
  char *stime = strsep(&str, ";");
  char *sclose = strsep(&str, ";");
  char *sopen = strsep(&str, ";");
  char *shi = strsep(&str, ";");
  char *slo = strsep(&str, ";");
  char *svol = strsep(&str, ";"); /* End */

  /* Check we got the right format */
  if(!stime || !sopen || !shi || !slo ||
     !sclose || !svol)
    goto err;
  
  /* Set values */
  sscanf(stime, "%d/%d/%d", &day, &month, &year);
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

  if(quotes_n3_alloc(n3, time, open, close, high, low, volume))
    return n3;
  
 err:
  return NULL;
}

static struct quotes_n3 *b4b_read(struct quotes *ctx)
{  
  char buf[256];
  struct quotes_n3 *n3;
  struct b4b *b = ctx->private;

  while(fgets(buf, sizeof buf, b->fp)){
    /* Prepare string & pre-filter */
    if(b4b_prepare_str(b, buf) < 0)
      continue;
    /* Parse n3 */
    if((n3 = b4b_parse_n3(b, buf))){
      PR_DBG("%s %s loaded\n", ctx->filename,
             time64_str_r(n3->time, GR_DAY, buf));
      /* We got a new candle */
      return n3;
    }
  }
  
  /* End of File */
  return NULL;
}

static int b4b_init(struct quotes *ctx)
{
  char dummy[256];
  
  struct b4b *b;
  
  if(!(b = calloc(1, sizeof *b)))
    return -ENOMEM;
  
  if(!(b->fp = fopen(ctx->filename, "r")))
    goto err;
  
  /* Ignore first line */
  fgets(dummy, sizeof(dummy), b->fp);
  ctx->private = b;
  return 0;

 err:
  free(b);
  return -1;
}

void b4b_release(struct quotes *ctx)
{
  struct b4b *b = ctx->private;
  if(b->fp) fclose(b->fp);
  free(b);
}

struct quotes_ops b4b_ops = {
  .init = b4b_init,
  .release = b4b_release,
  .read = b4b_read,
};
