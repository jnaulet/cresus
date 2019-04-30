/*
 * Cresus EVO - xtrade.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 02/05/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "xtrade.h"
#include "framework/verbose.h"
#include "framework/time64.h"

static time64_t xtrade_time(struct xtrade *ctx,
			       const char *str)
{
  int y, m, d;
  sscanf(str, "%d-%d-%d", &y, &m, &d);
  return TIME64_INIT(y, m, d, 0, 0, 0, 0);
}

static struct input_n3 *xtrade_read(struct input *in)
{
  struct input_n3 *n3;
  struct xtrade *ctx = (void*)in;
 
  /* Check for EOF at least */
  if(ctx->i < 0)
    goto err;
  
  json_value *o = ctx->data->u.array.values[ctx->i--];
  char *str = o->u.object.values[0].value->u.string.ptr;
  double open = o->u.object.values[1].value->u.dbl;
  double close = o->u.object.values[2].value->u.dbl;
  double low = o->u.object.values[3].value->u.dbl;
  double high = o->u.object.values[4].value->u.dbl;
  
  time64_t time = xtrade_time(ctx, str);
  if(input_n3_alloc(n3, time, GR_DAY,
		       open, close, high, low, 0.0))
    return n3;
  
 err:
  return NULL;
}

int xtrade_init(struct xtrade *ctx, const char *filename)
{
  int fd;
  size_t size;
  struct stat stat;

  /* init */
  __input_init__(ctx, xtrade_read);
  
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
  ctx->data = ctx->value->u.object.values[2].value;
  ctx->i = ctx->data->u.array.length - 1;
  
  close(fd);
  return 0;
  
 err:
  close(fd);
  PR_ERR("unable to load file %s\n", filename);
  return -1;
}

void xtrade_release(struct xtrade *ctx)
{  
  __input_release__(ctx);
  json_value_free(ctx->value);
  free(ctx->json);
}
