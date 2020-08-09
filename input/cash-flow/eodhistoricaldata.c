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

#include "framework/cash-flow.h"
#include "framework/verbose.h"
#include "framework/time64.h"

struct eodhistoricaldata {
  char statement[256];
  list_head_t(struct cash_flow_n3) list_cash_flow_n3s;
};

#define char_is_uppercase(x) (!((x) & (1 << 5)))
#define char_lower(x) ((x) | (1 << 5))

static char *java_to_c(const char *str)
{
  int len = strlen(str);
  static char buf[256];

  char *ptr = buf;
  
  for(int i = 0; i < len; i++){
    int c = str[i];
    if(char_is_uppercase(c)){
      *ptr++ = '_';
      *ptr++ = char_lower(c);
    }else
      *ptr++ = c;
  }

  *ptr = 0;
  return buf;
}

static time64_t json_str_to_time64(json_value *value)
{
  if(value->u.string.ptr) return time64_atot(value->u.string.ptr);
  else return 0;
}

static long long json_str_to_int(json_value *value)
{
  if(value->u.string.ptr)
    return strtoll(value->u.string.ptr, NULL, 10);
  
  return 0;
}

static int process_json_cash_flow_n3(struct eodhistoricaldata *e,
					 json_value *value)
{
  struct cash_flow_n3 *n3 = NULL;
  
  for(int i = 0; i < value->u.object.length; i++){
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;
    
    PR_DBG("DATA: %s\n", name);
    
    /* Create object */
    if(!strcasecmp("date", name))
      n3 = cash_flow_n3_alloc(n3, time64_atot(values->u.string.ptr));

    /* Real date might be different (+1 month probably) */
    if(!strcasecmp("filing_date", name) && values->u.string.ptr)
      n3->time = json_str_to_time64(values);

    /* Big data */
    if(!strcasecmp("investments", name)) n3->investments = json_str_to_int(values);
    if(!strcasecmp("changeToLiabilities", name)) n3->change_to_liabilities = json_str_to_int(values);
    if(!strcasecmp("totalCashflowsFromInvestingActivities", name)) n3->total_cashflows_from_investing_activities = json_str_to_int(values);
    if(!strcasecmp("netBorrowings", name)) n3->net_borrowings = json_str_to_int(values);
    if(!strcasecmp("totalCashFromFinancingActivities", name)) n3->total_cash_from_financing_activities = json_str_to_int(values);
    if(!strcasecmp("changeToOperatingActivities", name)) n3->change_to_operating_activities = json_str_to_int(values);
    if(!strcasecmp("netIncome", name)) n3->net_income = json_str_to_int(values);
    if(!strcasecmp("changeInCash", name)) n3->change_in_cash = json_str_to_int(values);
    if(!strcasecmp("totalCashFromOperatingActivities", name)) n3->total_cash_from_operating_activities = json_str_to_int(values);
    if(!strcasecmp("depreciation", name)) n3->depreciation = json_str_to_int(values);
    if(!strcasecmp("otherCashflowsFromInvestingActivities", name)) n3->other_cashflows_from_investing_activities = json_str_to_int(values);
    if(!strcasecmp("dividendsPaid", name)) n3->dividends_paid = json_str_to_int(values);
    if(!strcasecmp("changeToInventory", name)) n3->change_to_inventory = json_str_to_int(values);
    if(!strcasecmp("changeToAccountReceivables", name)) n3->change_to_account_receivables = json_str_to_int(values);
    if(!strcasecmp("salePurchaseOfStock", name)) n3->sale_purchase_of_stock = json_str_to_int(values);
    if(!strcasecmp("otherCashflowsFromFinancingActivities", name)) n3->other_cashflows_from_financing_activities = json_str_to_int(values);
    if(!strcasecmp("changeToNetincome", name)) n3->change_to_netincome = json_str_to_int(values);
    if(!strcasecmp("capitalExpenditures", name)) n3->capital_expenditures = json_str_to_int(values);
    if(!strcasecmp("changeReceivables", name)) n3->change_receivables = json_str_to_int(values);
    if(!strcasecmp("cashFlowsOtherOperating", name)) n3->cash_flows_other_operating = json_str_to_int(values);
    if(!strcasecmp("exchangeRateChanges", name)) n3->exchange_rate_changes = json_str_to_int(values);
    if(!strcasecmp("cashAndCashEquivalentsChanges", name)) n3->cash_and_cash_equivalents_changes = json_str_to_int(values);
    
    /* Generator */
    //PR_WARN("if(!strcasecmp(\"%s\", name)) n3->%s = json_str_to_int(values);\n", name, java_to_c(name));
  }
  
  /* Add entry to list (pile) */
  if(n3) list_add(&e->list_cash_flow_n3s, &n3->list);
  return 1;
}

static int process_json_entries(struct eodhistoricaldata *e, json_value *value)
{
  for(int i = 0; i < value->u.object.length; i++){
    struct cash_flow_n3 *n3 = NULL;
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;


    PR_DBG("%s: %s\n", e->statement, name);
    process_json_cash_flow_n3(e, values);
  }
  
  return 0;
}

static int process_json_object(struct eodhistoricaldata *e, json_value *value,
			       const char *parent)
{
  char json_node[256];
  static struct cash_flow_n3 *n3;

  /* Ignore sub-categories */
  if(value->type != json_object)
    return 0;
  
  for(int i = 0; i < value->u.object.length; i++){
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;

    /* Prepare json node name */
    if(parent == NULL) sprintf(json_node, "%s", name);
    else sprintf(json_node, "%s:%s", parent, name);
    
    PR_DBG("JSON node: %s\n", json_node);

    if(!strcasecmp(json_node, e->statement))
      return process_json_entries(e, values);
    
    /* Round trip */
    if(process_json_object(e, values, json_node) < 0){
      return -1;
    }
  }
  
  return 0;
}

static struct cash_flow_n3 *
eodhistoricaldata_read(struct cash_flow *ctx)
{
  struct eodhistoricaldata *e = ctx->private;
  struct cash_flow_n3 *n3 = (void*)list_pop(&e->list_cash_flow_n3s);

  if(!list_is_head(&n3->list))
    return n3;
  
  return NULL;
}

static int eodhistoricaldata_init(struct cash_flow *ctx)
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

  /* What to look for */
  strcpy(e->statement, "Financials:Cash_Flow:");
  if(ctx->period == QUARTERLY) strcat(e->statement, "quarterly");
  else strcat(e->statement, "yearly");
  
  /* Init e */
  list_head_init(&e->list_cash_flow_n3s);
  if(process_json_object(e, value, NULL) < 0)
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

static void eodhistoricaldata_release(struct cash_flow *ctx)
{
  struct eodhistoricaldata *e = ctx->private;
  if(e) free(e);
}

struct cash_flow_ops cash_flow_eodhistoricaldata_ops = {
  .init = eodhistoricaldata_init,
  .release = eodhistoricaldata_release,
  .read = eodhistoricaldata_read,
};
