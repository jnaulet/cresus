/*
 * Cresus EVO - b4b.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 12/30/2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>

#include "b4b.h"
#include "framework/verbose.h"

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

static struct input_n3 *
b4b_parse_n3(struct b4b *ctx, char *str)
{
  time64_t time = 0;
  int year, month, day;
  struct input_n3 *n3;
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

  if(input_n3_alloc(n3, time, open, close, high, low, volume))
    return n3;
  
 err:
  return NULL;
}

static struct input_n3 *b4b_read(struct input *in)
{
  struct b4b *ctx = (void*)in;
  
  char buf[256];
  struct input_n3 *n3;

  while(fgets(buf, sizeof buf, ctx->fp)){
    /* Prepare string & pre-filter */
    if(b4b_prepare_str(ctx, buf) < 0)
      continue;
    /* Parse n3 */
    if((n3 = b4b_parse_n3(ctx, buf))){
      PR_DBG("%s %s loaded\n", ctx->filename,
	     time64_str_r(n3->time, GR_DAY, buf));
      /* We got a new candle */
      return n3;
    }
  }
  
  /* End of File */
  return NULL;
}

int b4b_init(struct b4b *ctx, const char *filename)
{
  char dummy[256];
  
  /* init */
  __input_init__(ctx, b4b_read);

  strncpy(ctx->filename, filename, sizeof(ctx->filename));
  if(!(ctx->fp = fopen(ctx->filename, "r"))){
    PR_ERR("unable to open file %s\n", ctx->filename);
    return -1;
  }

  /* Ignore first line */
  fgets(dummy, sizeof(dummy), ctx->fp);
  
  return 0;
}

void b4b_release(struct b4b *ctx)
{
  __input_release__(ctx);
  if(ctx->fp) fclose(ctx->fp);
}
