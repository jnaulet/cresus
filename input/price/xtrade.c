/*
 * Cresus EVO - xtrade.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 02/05/2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <json-parser/json.h>

#include "framework/price.h"
#include "framework/verbose.h"
#include "framework/time64.h"

struct xtrade {
  int i;
  json_char *json;
  json_value *value;
  json_value *data;
};

static time64_t xtrade_time(struct xtrade *ctx,
                            const char *str)
{
  int y, m, d;
  sscanf(str, "%d-%d-%d", &y, &m, &d);
  return TIME64_INIT(y, m, d, 0, 0, 0, 0);
}

static struct price_n3 *xtrade_read(struct price *ctx)
{
  struct price_n3 *n3;
  struct xtrade *x = ctx->private;
  
  /* Check for EOF at least */
  if(x->i < 0)
    goto err;
  
  json_value *o = x->data->u.array.values[x->i--];
  char *str = o->u.object.values[0].value->u.string.ptr;
  double open = o->u.object.values[1].value->u.dbl;
  double close = o->u.object.values[2].value->u.dbl;
  double low = o->u.object.values[3].value->u.dbl;
  double high = o->u.object.values[4].value->u.dbl;
  
  time64_t time = xtrade_time(x, str);
  if(price_n3_alloc(n3, time, open, close, high, low, 0.0))
    return n3;
  
 err:
  return NULL;
}

int xtrade_init(struct price *ctx)
{
  int fd;
  size_t size;
  struct stat stat;
  struct xtrade *x;

  /* init */
  if(!(x = calloc(1, sizeof(*x))))
    return -ENOMEM;

  /* open */
  if((fd = open(ctx->filename, O_RDONLY)) < 0)
    goto err;
  /* Init +exception */
  if(fstat(fd, &stat) < 0)
    goto err2;
  /* allocate RAM */
  size = stat.st_size;
  if(!(x->json = malloc(size)))
    goto err2;
  /* Load entire file to RAM */
  if(read(fd, (void*)x->json, size) != size)
    goto err3;
  /* json parse */
  if(!(x->value = json_parse(x->json, size)))
    goto err3;
  
  /* Ok */
  x->data = x->value->u.object.values[2].value;
  x->i = x->data->u.array.length - 1;
  ctx->private = x;
  
  close(fd);
  return 0;

 err3:
  free(x->json);
 err2:
  close(fd);
 err:
  free(x);
  return -1;
}

void xtrade_release(struct price *ctx)
{
  struct xtrade *x = ctx->private;
  json_value_free(x->value);
  free(x->json);
  free(x);
}

struct price_ops xtrade_ops = {
  .init = xtrade_init,
  .release = xtrade_release,
  .read = xtrade_read,
};
