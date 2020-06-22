/*
 * Cresus EVO - euronext.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 24/07/2018
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

struct euronext {
  /* internals */
  json_char *json;
  json_value *value;
  /* Our mix */
  int i;
  size_t len;
  json_value *data;
};

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

static struct price_n3 *euronext_read(struct price *ctx)
{
  struct price_n3 *n3;
  struct euronext *e = ctx->private;
  
  /* Check for EOF at least */
  if(e->i >= e->len)
    goto err;
  
  json_value *o = e->data->u.array.values[e->i++];
  char *date = o->u.object.values[2].value->u.string.ptr;
  char *sopen = o->u.object.values[3].value->u.string.ptr;
  char *shigh = o->u.object.values[4].value->u.string.ptr;
  char *slow = o->u.object.values[5].value->u.string.ptr;
  char *sclose = o->u.object.values[6].value->u.string.ptr;
  
  time64_t time = euronext_time(e, date);
  double open = euronext_dbl(e, sopen);
  double high = euronext_dbl(e, shigh);
  double low = euronext_dbl(e, slow);
  double close = euronext_dbl(e, sclose);
  
  if(price_n3_alloc(n3, time, open, close, high, low, 0.0))
    return n3;
  
 err:
  return NULL;
}

int euronext_init(struct price *ctx)
{
  int fd;
  size_t size;
  struct stat stat;
  struct euronext *e;

  /* Alloc */
  if(!(e = calloc(1, sizeof *e)))
    return -ENOMEM;
  
  /* Open file */
  if((fd = open(ctx->filename, O_RDONLY)) < 0)
    goto err;
  /* Init +exception */
  if(fstat(fd, &stat) < 0)
    goto err2;
  /* allocate RAM */
  size = stat.st_size;
  if(!(e->json = malloc(size)))
    goto err2;
  /* Load entire file to RAM */
  if(read(fd, (void*)e->json, size) != size)
    goto err3;
  /* json parse */
  if(!(e->value = json_parse(e->json, size)))
    goto err3;
  
  /* Ok */
  e->data = e->value->u.object.values->value;
  e->len = e->data->u.array.length;
  e->i = 0;
  
  ctx->private = e;
  close(fd);
  return 0;
  
 err3:
  free(e->json);
 err2:
  close(fd);
 err:
  free(e);
  return -1;
}

void euronext_release(struct price *ctx)
{
  struct euronext *e = ctx->private;
  json_value_free(e->value);
  free(e->json);
  free(e);
}

struct price_ops euronext_ops = {
  .init = euronext_init,
  .release = euronext_release,
  .read = euronext_read,
};
