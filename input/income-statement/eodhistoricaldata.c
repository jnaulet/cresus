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

#include "framework/income-statement.h"
#include "framework/verbose.h"
#include "framework/time.h"

struct eodhistoricaldata {
  char statement[256];
  list_head_t(struct income_statement_n3) list_income_statement_n3s;
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

static time_t json_str_to_time(json_value *value)
{
  if(value->u.string.ptr) return time_atot(value->u.string.ptr);
  else return 0;
}

static long long json_str_to_int(json_value *value)
{
  if(value->u.string.ptr)
    return strtoll(value->u.string.ptr, NULL, 10);
  
  return 0;
}

static int process_json_income_statement_n3(struct eodhistoricaldata *e,
					 json_value *value)
{
  struct income_statement_n3 *n3 = NULL;
  
  for(int i = 0; i < value->u.object.length; i++){
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;
    
    PR_DBG("DATA: %s\n", name);
    
    /* Create object */
    if(!strcasecmp("date", name))
      n3 = income_statement_n3_alloc(n3, time_atot(values->u.string.ptr));

#if 0
    /* Real date might be different (+1 month probably) */
    if(!strcasecmp("filing_date", name) && values->u.string.ptr)
      n3->time = json_str_to_time(values);
#endif

    /* Big data */
    if(!strcasecmp("researchDevelopment", name)) n3->research_development = json_str_to_int(values);
    if(!strcasecmp("effectOfAccountingCharges", name)) n3->effect_of_accounting_charges = json_str_to_int(values);
    if(!strcasecmp("incomeBeforeTax", name)) n3->income_before_tax = json_str_to_int(values);
    if(!strcasecmp("minorityInterest", name)) n3->minority_interest = json_str_to_int(values);
    if(!strcasecmp("netIncome", name)) n3->net_income = json_str_to_int(values);
    if(!strcasecmp("sellingGeneralAdministrative", name)) n3->selling_general_administrative = json_str_to_int(values);
    if(!strcasecmp("grossProfit", name)) n3->gross_profit = json_str_to_int(values);
    if(!strcasecmp("ebit", name)) n3->ebit = json_str_to_int(values);
    if(!strcasecmp("nonOperatingIncomeNetOther", name)) n3->non_operating_income_net_other = json_str_to_int(values);
    if(!strcasecmp("operatingIncome", name)) n3->operating_income = json_str_to_int(values);
    if(!strcasecmp("otherOperatingExpenses", name)) n3->other_operating_expenses = json_str_to_int(values);
    if(!strcasecmp("interestExpense", name)) n3->interest_expense = json_str_to_int(values);
    if(!strcasecmp("taxProvision", name)) n3->tax_provision = json_str_to_int(values);
    if(!strcasecmp("interestIncome", name)) n3->interest_income = json_str_to_int(values);
    if(!strcasecmp("netInterestIncome", name)) n3->net_interest_income = json_str_to_int(values);
    if(!strcasecmp("extraordinaryItems", name)) n3->extraordinary_items = json_str_to_int(values);
    if(!strcasecmp("nonRecurring", name)) n3->non_recurring = json_str_to_int(values);
    if(!strcasecmp("otherItems", name)) n3->other_items = json_str_to_int(values);
    if(!strcasecmp("incomeTaxExpense", name)) n3->income_tax_expense = json_str_to_int(values);
    if(!strcasecmp("totalRevenue", name)) n3->total_revenue = json_str_to_int(values);
    if(!strcasecmp("totalOperatingExpenses", name)) n3->total_operating_expenses = json_str_to_int(values);
    if(!strcasecmp("costOfRevenue", name)) n3->cost_of_revenue = json_str_to_int(values);
    if(!strcasecmp("totalOtherIncomeExpenseNet", name)) n3->total_other_income_expense_net = json_str_to_int(values);
    if(!strcasecmp("discontinuedOperations", name)) n3->discontinued_operations = json_str_to_int(values);
    if(!strcasecmp("netIncomeFromContinuingOps", name)) n3->net_income_from_continuing_ops = json_str_to_int(values);
    if(!strcasecmp("netIncomeApplicableToCommonShares", name)) n3->net_income_applicable_to_common_shares = json_str_to_int(values);
    if(!strcasecmp("preferredStockAndOtherAdjustments", name)) n3->preferred_stock_and_other_adjustments = json_str_to_int(values);

    /* Generator */
    //PR_WARN("if(!strcasecmp(\"%s\", name)) n3->%s = json_str_to_int(values);\n", name, java_to_c(name));
  }
  
  /* Add entry to list (pile) */
  if(n3) list_add(&e->list_income_statement_n3s, &n3->list);
  return 1;
}

static int process_json_entries(struct eodhistoricaldata *e, json_value *value)
{
  for(int i = 0; i < value->u.object.length; i++){
    struct income_statement_n3 *n3 = NULL;
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;


    PR_DBG("%s: %s\n", e->statement, name);
    process_json_income_statement_n3(e, values);
  }
  
  return 0;
}

static int process_json_object(struct eodhistoricaldata *e, json_value *value,
			       const char *parent)
{
  char json_node[256];
  static struct income_statement_n3 *n3;

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

static struct income_statement_n3 *
eodhistoricaldata_read(struct income_statement *ctx)
{
  struct eodhistoricaldata *e = ctx->private;
  struct income_statement_n3 *n3 = (void*)list_pop(&e->list_income_statement_n3s);

  if(!list_is_head(&n3->list))
    return n3;
  
  return NULL;
}

static int eodhistoricaldata_init(struct income_statement *ctx)
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
  strcpy(e->statement, "Financials:Income_Statement:");
  if(ctx->period == QUARTERLY) strcat(e->statement, "quarterly");
  else strcat(e->statement, "yearly");
  
  /* Init e */
  list_head_init(&e->list_income_statement_n3s);
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

static void eodhistoricaldata_release(struct income_statement *ctx)
{
  struct eodhistoricaldata *e = ctx->private;
  if(e) free(e);
}

struct income_statement_ops income_statement_eodhistoricaldata_ops = {
  .init = eodhistoricaldata_init,
  .release = eodhistoricaldata_release,
  .read = eodhistoricaldata_read,
};
