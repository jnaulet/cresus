/*
 * Cresus EVO - euronext.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 24/07/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "euronext.h"
#include "framework/verbose.h"
#include "framework/time64.h"

static time64_t euronext_time(struct euronext *ctx,
				 const char *str)
{
  int y, m, d;
  sscanf(str, "%d/%d/%d", &d, &m, &y);
  return TIME64_INIT(y, m, d, 0, 0, 0, 0);
}

static double euronext_dbl(struct euronext *ctx, char *str)
{
  char *stringp = str;
  int t = 0, h = 0, c = 0;

  char *thousands = strsep(&stringp, ".");
  if(!stringp) stringp = str;
  else sscanf(thousands, "%d", &t);
  
  char *hundreds = strsep(&stringp, ",");
  char *decimal = stringp;
  
  sscanf(hundreds, "%d", &h);
  sscanf(decimal, "%d", &c);
  
  return ((double)t * 1000.0) + (double)h + ((double)c / 100.0);
}

static struct input_n3 *euronext_read(struct input *in)
{
  struct input_n3 *n3;
  struct euronext *ctx = (void*)in;
  
  /* Check for EOF at least */
  if(ctx->i >= ctx->len)
    goto err;
  
  json_value *o = ctx->data->u.array.values[ctx->i++];
  char *date = o->u.object.values[2].value->u.string.ptr;
  char *sopen = o->u.object.values[3].value->u.string.ptr;
  char *shigh = o->u.object.values[4].value->u.string.ptr;
  char *slow = o->u.object.values[5].value->u.string.ptr;
  char *sclose = o->u.object.values[6].value->u.string.ptr;
  
  time64_t time = euronext_time(ctx, date);
  double open = euronext_dbl(ctx, sopen);
  double high = euronext_dbl(ctx, shigh);
  double low = euronext_dbl(ctx, slow);
  double close = euronext_dbl(ctx, sclose);
  
  if(input_n3_alloc(n3, time, open, close, high, low, 0.0))
    return n3;
  
 err:
  return NULL;
}

int euronext_init(struct euronext *ctx, const char *filename)
{
  int fd;
  size_t size;
  struct stat stat;

  /* init */
  __input_init__(ctx, euronext_read);
  
  /* internals */
  ctx->i = 0;

  if((fd = open(filename, O_RDONLY)) < 0)
    return -1;

  /* Init +exception */
  if(fstat(fd, &stat) < 0)
    goto err;
  /* allocate RAM */
  size = stat.st_size;
  if(!(ctx->json = malloc(size)))
    goto err;
  /* Load entire file to RAM */
  if(read(fd, (void*)ctx->json, size) != size)
    goto err;
  /* json parse */
  if(!(ctx->value = json_parse(ctx->json, size)))
    goto err;
  
  /* Ok */
  ctx->data = ctx->value->u.object.values->value;
  ctx->len = ctx->data->u.array.length;
  
  close(fd);
  return 0;
  
 err:
  close(fd);
  PR_ERR("unable to load file %s\n", filename);
  return -1;
}

void euronext_release(struct euronext *ctx)
{  
  __input_release__(ctx);
  json_value_free(ctx->value);
  free(ctx->json);
}
