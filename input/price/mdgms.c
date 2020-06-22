/*
 * Cresus EVO - mdgms.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/01/2018
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

struct mdgms {
  int i;
  json_char *json;
  json_value *value;
};

static struct price_n3 *mdgms_read(struct price *ctx)
{
  struct price_n3 *n3;
  struct mdgms *m = ctx->private;
  
  /* Values */
  json_value *ts = m->value->u.object.values[0].value;
  json_value *op = m->value->u.object.values[1].value;
  json_value *hi = m->value->u.object.values[2].value;
  json_value *lo = m->value->u.object.values[3].value;
  json_value *cl = m->value->u.object.values[4].value;
  json_value *vl = m->value->u.object.values[5].value;
  
  /* Check */
  __try__(!ts || !op || !hi || !lo || !cl || !vl, err); /* Parse error */
  __try__(m->i >= ts->u.array.length, err); /* EOF */
  
  /* How to check ? */
  time_t t = ts->u.array.values[m->i]->u.integer;
  double open = op->u.array.values[m->i]->u.dbl;
  double high = hi->u.array.values[m->i]->u.dbl;
  double low = lo->u.array.values[m->i]->u.dbl;
  double close = cl->u.array.values[m->i]->u.dbl;
  double vol = vl->u.array.values[m->i]->u.integer;

  /* Increment */
  m->i++;
  
  time64_t time = time64_epoch(t);
  if(price_n3_alloc(n3, time, open, close, high, low, vol))
    return n3;
  
 __catch__(err):
  return NULL;
}

static int mdgms_init(struct price *ctx)
{
  int fd;
  size_t size;
  struct mdgms *m;
  struct stat stat;

  /* alloc context */
  if(!(m = calloc(1, sizeof(*m))))
    return -ENOMEM;

  /* open */
  if((fd = open(ctx->filename, O_RDONLY)) < 0)
    goto err;
  /* Init +exception */
  if(fstat(fd, &stat) < 0)
    goto err2;
  /* allocate RAM */
  size = stat.st_size;
  if(!(m->json = malloc(size)))
    goto err2;
  /* Load entire file to RAM */
  if(read(fd, (void*)m->json, size) != size)
    goto err3;
  /* json parse */
  if(!(m->value = json_parse(m->json, size)))
    goto err3;
  
  /* internals */
  m->i = 0;
  ctx->private = m;
  
  /* Ok */
  close(fd);
  return 0;

 err3:
  free(m->json);
 err2:
  close(fd);
 err:
  free(m);
  return -1;
}

static void mdgms_release(struct price *ctx)
{
  struct mdgms *m = ctx->private;
  json_value_free(m->value);
  free(m->json);
  free(m);
}

struct price_ops mdgms_ops = {
  .init = mdgms_init,
  .release = mdgms_release,
  .read = mdgms_read,
};
