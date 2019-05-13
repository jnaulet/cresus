/*
 * Cresus EVO - kraken.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 23/04/2019
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "kraken.h"
#include "framework/verbose.h"
#include "framework/time64.h"

static double kraken_dbl(struct kraken *ctx, char *str)
{
  double d;
  sscanf(str, "%lf", &d);
  
  return d;
}

/*
 * Format : <time>, <open>, <high>, <low>, <close>, <vwap>, <volume>, <count>
 */
static struct input_n3 *kraken_read(struct input *in)
{
  struct input_n3 *n3;
  struct kraken *ctx = (void*)in;
 
  /* Check for EOF at least */
  if(ctx->i >= ctx->len)
    goto err;
  
  json_value *o = ctx->data->u.array.values[ctx->i++];
  time_t date = o->u.array.values[0]->u.integer; /* Epoch */
  char *sopen = o->u.array.values[1]->u.string.ptr;
  char *shigh = o->u.array.values[2]->u.string.ptr;
  char *slow = o->u.array.values[3]->u.string.ptr;
  char *sclose = o->u.array.values[4]->u.string.ptr;
  char *svol = o->u.array.values[6]->u.string.ptr;
  
  time64_t time = time64_epoch(date);
  double open = kraken_dbl(ctx, sopen);
  double high = kraken_dbl(ctx, shigh);
  double low = kraken_dbl(ctx, slow);
  double close = kraken_dbl(ctx, sclose);
  double vol = kraken_dbl(ctx, svol);
  
  if(input_n3_alloc(n3, time, open, close, high, low, vol))
    return n3;
  
 err:
  return NULL;
}

int kraken_init(struct kraken *ctx, const char *filename)
{
  int fd;
  size_t size;
  struct stat stat;

  /* init */
  __input_init__(ctx, kraken_read);
  
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
  ctx->data = ctx->value->u.object.values[1].value; /* "result" value */
  ctx->data = ctx->data->u.object.values[0].value; /* "pair" value */
  ctx->len = ctx->data->u.array.length;
  
  close(fd);
  return 0;
  
 err:
  close(fd);
  PR_ERR("unable to load file %s\n", filename);
  return -1;
}

void kraken_release(struct kraken *ctx)
{  
  __input_release__(ctx);
  json_value_free(ctx->value);
  free(ctx->json);
}
