/*
 * Cresus EVO - mdgms.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/01/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "mdgms.h"
#include "engine/candle.h"
#include "framework/verbose.h"

static struct timeline_entry *mdgms_read(struct input *in)
{
  struct candle *c;
  struct mdgms *ctx = __input_self__(in);
  
  /* TODO: check !!! */
  json_value *ts = ctx->value->u.object.values[0].value;
  json_value *op = ctx->value->u.object.values[1].value;
  json_value *hi = ctx->value->u.object.values[2].value;
  json_value *lo = ctx->value->u.object.values[3].value;
  json_value *cl = ctx->value->u.object.values[4].value;
  json_value *vl = ctx->value->u.object.values[5].value;
  
  /* Check for EOF */
  if(ctx->i >= ts->u.array.length)
    goto err;
  
  /* ! */
  time_t t = ts->u.array.values[ctx->i]->u.integer;
  double open = op->u.array.values[ctx->i]->u.dbl;
  double high = hi->u.array.values[ctx->i]->u.dbl;
  double low = lo->u.array.values[ctx->i]->u.dbl;
  double close = cl->u.array.values[ctx->i]->u.dbl;
  double vol = vl->u.array.values[ctx->i]->u.integer;

  /* Increment */
  ctx->i++;
  
  time_info_t time = time_info_epoch(t);
  if(candle_alloc(c, time, GRANULARITY_DAY,
                  open, close, high, low, vol))
    return __timeline_entry__(c);
  
 err:
  return NULL;
}

int mdgms_init(struct mdgms *ctx, const char *filename)
{
  int fd;
  size_t size;
  struct stat stat;

  /* super */
  __input_super__(ctx, mdgms_read);
  
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
  close(fd);
  return 0;
  
 err:
  close(fd);
  PR_ERR("unable to load file %s\n", filename);
  return -1;
}

void mdgms_release(struct mdgms *ctx)
{  
  __input_release__(ctx);
  json_value_free(ctx->value);
  free(ctx->json);
}
