/*
 * Cresus EVO - kraken.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 23/04/2019
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

#include "framework/list.h"
#include "framework/price.h"
#include "framework/verbose.h"
#include "framework/time64.h"

typedef enum {
  KRAKEN_STATE_INIT,
  KRAKEN_STATE_ERROR,
  KRAKEN_STATE_RESULT,
} kraken_state_t;

struct kraken {
  json_char *json;
  json_value *value;
  /* State machine */
  kraken_state_t state;
  /* Tmp list */
  list_head_t(struct price_n3) list_price_n3s;
};

static double kraken_dbl(struct kraken *ctx, char *str)
{
  double d;
  sscanf(str, "%lf", &d);
  
  return d;
}

/* For recursion */
static int kraken_process_json_value(struct kraken *k, json_value *value);

static int kraken_process_json_object(struct kraken *k, json_value *value)
{
  for(int i = 0; i < value->u.object.length; i++){
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;
    
    /* Error */
    if(!strcasecmp("error", name))
      k->state = KRAKEN_STATE_ERROR;
    /* Result */
    if(!strcasecmp("result", name))
      k->state = KRAKEN_STATE_RESULT;
    /* Last entry */
    if(!strcasecmp("last", name))
      return 0;

    /* Round trip */
    if(kraken_process_json_value(k, values) < 0){
      return -1;
    }
  }
  
  return 0;
}

static int kraken_process_json_array(struct kraken *k, json_value *value)
{
  struct price_n3 *n3;
  json_value **values = value->u.array.values;

  if(k->state == KRAKEN_STATE_ERROR){
    if(value->u.array.length > 0){
      PR_ERR("kraken: error in json response\n");
      return -1;
    }
  }
  
  if(k->state == KRAKEN_STATE_RESULT &&
     value->u.array.length == 8){
    /* We got pairs here */    
    time_t date = values[0]->u.integer; /* Epoch */
    char *sopen = values[1]->u.string.ptr;
    char *shigh = values[2]->u.string.ptr;
    char *slow = values[3]->u.string.ptr;
    char *sclose = values[4]->u.string.ptr;
    char *svol = values[6]->u.string.ptr;
    
    time64_t time = time64_epoch(date);
    double open = kraken_dbl(k, sopen);
    double high = kraken_dbl(k, shigh);
    double low = kraken_dbl(k, slow);
    double close = kraken_dbl(k, sclose);
    double vol = kraken_dbl(k, svol);

    if(price_n3_alloc(n3, time, open, close, high, low, vol))
      list_add_tail(&k->list_price_n3s, &n3->list);

    return 0;
  }

  for(int i = 0; i < value->u.array.length; i++)
    kraken_process_json_value(k, values[i]);
  
  return 0;
}

static int kraken_process_json_value(struct kraken *k, json_value *value)
{
  switch(value->type){
  case json_object: return kraken_process_json_object(k, value);
  case json_array: return kraken_process_json_array(k, value);
  default: PR_INFO("type: %d\n", value->type); break;
  }

  return 0;
}

/*
 * Format : <time>, <open>, <high>, <low>, <close>, <vwap>, <volume>, <count>
 */
static struct price_n3 *kraken_read(struct price *ctx)
{
  struct kraken *k = ctx->private; 
  struct price_n3 *n3 = (void*)list_pop(&k->list_price_n3s);
  
  if(!list_is_head(&n3->list))
    return n3;
  
  return NULL;
}

static int kraken_init(struct price *ctx)
{
  int fd;
  size_t size;
  struct stat stat;
  struct kraken *k;

  if(!(k = calloc(1, sizeof(*k))))
    return -ENOMEM;

  /* open*/
  if((fd = open(ctx->filename, O_RDONLY)) < 0)
    goto err;
  /* Init +exception */
  if(fstat(fd, &stat) < 0)
    goto err2;
  /* allocate RAM */
  size = stat.st_size;
  if(!(k->json = malloc(size)))
    goto err2;
  /* Load entire file to RAM */
  if(read(fd, (void*)k->json, size) != size)
    goto err3;
  /* json parse */
  if(!(k->value = json_parse(k->json, size)))
    goto err3;
  
  close(fd);
  ctx->private = k;
  k->state = KRAKEN_STATE_INIT;
  list_head_init(&k->list_price_n3s);
  return kraken_process_json_value(k, k->value);

 err3:
  free(k->json);
 err2:
  close(fd);
 err:
  free(k);
  return -1;
}

static void kraken_release(struct price *ctx)
{
  struct kraken *k = ctx->private;
  json_value_free(k->value);
  free(k->json);
  free(k);
}

struct price_ops kraken_ops = {
  .init = kraken_init,
  .release = kraken_release,
  .read = kraken_read,
};
