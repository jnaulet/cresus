/*
 * Cresus - eodhistoricaldata.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/07/2020
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

#include "framework/balance-sheet.h"
#include "framework/verbose.h"
#include "framework/time64.h"

typedef enum {
  INIT,
  FINANCIALS,
  BALANCE_SHEET,
  QUARTERLY,
  ANNUALLY
} eodhistoricaldata_state_t;

struct eodhistoricaldata {
  eodhistoricaldata_state_t state;
  list_head_t(struct balance_sheet_n3) list_balance_sheet_n3s;
};

static int process_json_value(struct eodhistoricaldata *e, json_value *value);

static int process_json_object(struct eodhistoricaldata *e, json_value *value)
{  
  for(int i = 0; i < value->u.object.length; i++){
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;

    PR_DBG("name: %s\n", name);
    
    /* Financials */
    if(!strcasecmp("Financials", name))
      e->state = FINANCIALS;
    /* Balance_Sheet */
    if(!strcasecmp("Balance_Sheet", name))
      e->state = BALANCE_SHEET;
    /* Quarterly */
    if(!strcasecmp("Quarterly", name))
      e->state = QUARTERLY;
  }
    
  return 0;
}

static int process_json_array(struct eodhistoricaldata *e, json_value *value)
{
  return 0;
}

static int process_json_value(struct eodhistoricaldata *e, json_value *value)
{
  switch(value->type){
  case json_object: return process_json_object(e, value);
  case json_array: return process_json_array(e, value);
  default: PR_WARN("Unknown type: %d\n", value->type); break;
  }
  
  return 0;
}

static struct balance_sheet_n3 *
eodhistoricaldata_read(struct balance_sheet *ctx)
{
  return NULL;
}

static int eodhistoricaldata_init(struct balance_sheet *ctx)
{
  int fd;
  size_t size;
  struct stat stat;

  json_char *json;
  json_value *value;

  struct eodhistoricaldata *e;
  if(!(e = calloc(1, sizeof(*e))))
    return -ENOMEM;
  
  /* open*/
  if((fd = open(ctx->filename, O_RDONLY)) < 0)
    goto err;
  /* Init +exception */
  if(fstat(fd, &stat) < 0)
    goto err2;
  /* allocate RAM */
  size = stat.st_size;
  if(!(json = malloc(size)))
    goto err2;
  /* Load entire file to RAM */
  if(read(fd, (void*)json, size) != size)
    goto err3;
  /* json parse */
  if(!(value = json_parse(json, size)))
    goto err3;

  /* Init e */
  e->state = INIT;
  list_head_init(&e->list_balance_sheet_n3s);
  if(process_json_value(e, value) < 0)
    goto err4;

  /* Remember e */
  ctx->private = e;
  
  /* Done */
  json_value_free(value);
  free(json);
  close(fd);
  return 0;
  
 err4:
  json_value_free(value);
 err3:
  free(json);
 err2:
  close(fd);
 err:
  free(e);
  return -1;
}

static void eodhistoricaldata_release(struct balance_sheet *ctx)
{
  struct eodhistoricaldata *e = ctx->private;
  if(e) free(e);
}

struct balance_sheet_ops eodhistoricaldata_ops = {
  .init = eodhistoricaldata_init,
  .release = eodhistoricaldata_release,
  .read = eodhistoricaldata_read,
};
