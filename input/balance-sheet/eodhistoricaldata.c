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

struct eodhistoricaldata {
  char statement[256];
  list_head_t(struct balance_sheet_n3) list_balance_sheet_n3s;
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

static int process_json_balance_sheet_n3(struct eodhistoricaldata *e,
					 json_value *value)
{
  struct balance_sheet_n3 *n3 = NULL;
  
  for(int i = 0; i < value->u.object.length; i++){
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;
    
    PR_DBG("DATA: %s\n", name);
    
    /* Create object */
    if(!strcasecmp("date", name))
      n3 = balance_sheet_n3_alloc(n3, time64_atot(values->u.string.ptr));

    /* Real date might be different (+1 month probably) */
    if(!strcasecmp("filing_date", name) && values->u.string.ptr)
      n3->time = json_str_to_time64(values);

    /* Big data */
    if(!strcasecmp("totalAssets", name)) n3->total_assets = json_str_to_int(values);
    if(!strcasecmp("intangibleAssets", name)) n3->intangible_assets = json_str_to_int(values);
    if(!strcasecmp("earningAssets", name)) n3->earning_assets = json_str_to_int(values);
    if(!strcasecmp("otherCurrentAssets", name)) n3->other_current_assets = json_str_to_int(values);
    if(!strcasecmp("totalLiab", name)) n3->total_liab = json_str_to_int(values);
    if(!strcasecmp("totalStockholderEquity", name)) n3->total_stockholder_equity = json_str_to_int(values);
    if(!strcasecmp("deferredLongTermLiab", name)) n3->deferred_long_term_liab = json_str_to_int(values);
    if(!strcasecmp("otherCurrentLiab", name)) n3->other_current_liab = json_str_to_int(values);
    if(!strcasecmp("commonStock", name)) n3->common_stock = json_str_to_int(values);
    if(!strcasecmp("retainedEarnings", name)) n3->retained_earnings = json_str_to_int(values);
    if(!strcasecmp("otherLiab", name)) n3->other_liab = json_str_to_int(values);
    if(!strcasecmp("goodWill", name)) n3->good_will = json_str_to_int(values);
    if(!strcasecmp("otherAssets", name)) n3->other_assets = json_str_to_int(values);
    if(!strcasecmp("cash", name)) n3->cash = json_str_to_int(values);
    if(!strcasecmp("totalCurrentLiabilities", name)) n3->total_current_liabilities = json_str_to_int(values);
    if(!strcasecmp("shortTermDebt", name)) n3->short_term_debt = json_str_to_int(values);
    if(!strcasecmp("shortLongTermDebt", name)) n3->short_long_term_debt = json_str_to_int(values);
    if(!strcasecmp("shortLongTermDebtTotal", name)) n3->short_long_term_debt_total = json_str_to_int(values);
    if(!strcasecmp("otherStockholderEquity", name)) n3->other_stockholder_equity = json_str_to_int(values);
    if(!strcasecmp("propertyPlantEquipment", name)) n3->property_plant_equipment = json_str_to_int(values);
    if(!strcasecmp("totalCurrentAssets", name)) n3->total_current_assets = json_str_to_int(values);
    if(!strcasecmp("longTermInvestments", name)) n3->long_term_investments = json_str_to_int(values);
    if(!strcasecmp("netTangibleAssets", name)) n3->net_tangible_assets = json_str_to_int(values);
    if(!strcasecmp("shortTermInvestments", name)) n3->short_term_investments = json_str_to_int(values);
    if(!strcasecmp("netReceivables", name)) n3->net_receivables = json_str_to_int(values);
    if(!strcasecmp("longTermDebt", name)) n3->long_term_debt = json_str_to_int(values);
    if(!strcasecmp("inventory", name)) n3->inventory = json_str_to_int(values);
    if(!strcasecmp("accountsPayable", name)) n3->accounts_payable = json_str_to_int(values);
    if(!strcasecmp("totalPermanentEquity", name)) n3->total_permanent_equity = json_str_to_int(values);
    if(!strcasecmp("noncontrollingInterestInConsolidatedEntity", name)) n3->noncontrolling_interest_in_consolidated_entity = json_str_to_int(values);
    if(!strcasecmp("temporaryEquityRedeemableNoncontrollingInterests", name)) n3->temporary_equity_redeemable_noncontrolling_interests = json_str_to_int(values);
    if(!strcasecmp("accumulatedOtherComprehensiveIncome", name)) n3->accumulated_other_comprehensive_income = json_str_to_int(values);
    if(!strcasecmp("additionalPaidInCapital", name)) n3->additional_paid_in_capital = json_str_to_int(values);
    if(!strcasecmp("commonStockTotalEquity", name)) n3->common_stock_total_equity = json_str_to_int(values);
    if(!strcasecmp("preferredStockTotalEquity", name)) n3->preferred_stock_total_equity = json_str_to_int(values);
    if(!strcasecmp("retainedEarningsTotalEquity", name)) n3->retained_earnings_total_equity = json_str_to_int(values);
    if(!strcasecmp("treasuryStock", name)) n3->treasury_stock = json_str_to_int(values);
    if(!strcasecmp("accumulatedAmortization", name)) n3->accumulated_amortization = json_str_to_int(values);
    if(!strcasecmp("nonCurrrentAssetsOther", name)) n3->non_currrent_assets_other = json_str_to_int(values);
    if(!strcasecmp("deferredLongTermAssetCharges", name)) n3->deferred_long_term_asset_charges = json_str_to_int(values);
    if(!strcasecmp("nonCurrentAssetsTotal", name)) n3->non_current_assets_total = json_str_to_int(values);
    if(!strcasecmp("capitalLeaseObligations", name)) n3->capital_lease_obligations = json_str_to_int(values);
    if(!strcasecmp("longTermDebtTotal", name)) n3->long_term_debt_total = json_str_to_int(values);
    if(!strcasecmp("nonCurrentLiabilitiesOther", name)) n3->non_current_liabilities_other = json_str_to_int(values);
    if(!strcasecmp("nonCurrentLiabilitiesTotal", name)) n3->non_current_liabilities_total = json_str_to_int(values);
    if(!strcasecmp("negativeGoodwill", name)) n3->negative_goodwill = json_str_to_int(values);
    if(!strcasecmp("warrants", name)) n3->warrants = json_str_to_int(values);
    if(!strcasecmp("preferredStockRedeemable", name)) n3->preferred_stock_redeemable = json_str_to_int(values);
    if(!strcasecmp("capitalSurpluse", name)) n3->capital_surpluse = json_str_to_int(values);
    if(!strcasecmp("liabilitiesAndStockholdersEquity", name)) n3->liabilities_and_stockholders_equity = json_str_to_int(values);
    if(!strcasecmp("cashAndShortTermInvestments", name)) n3->cash_and_short_term_investments = json_str_to_int(values);
    if(!strcasecmp("propertyPlantAndEquipmentGross", name)) n3->property_plant_and_equipment_gross = json_str_to_int(values);
    if(!strcasecmp("accumulatedDepreciation", name)) n3->accumulated_depreciation = json_str_to_int(values);
    if(!strcasecmp("commonStockSharesOutstanding", name)) n3->common_stock_shares_outstanding = json_str_to_int(values);
    
    /* Generator */
    //PR_WARN("if(!strcasecmp(\"%s\", name)) n3->%s = json_str_to_int(values);\n", name, java_to_c(name));
  }
  
  /* Add entry to list (pile) */
  if(n3) list_add(&e->list_balance_sheet_n3s, &n3->list);
  return 1;
}

static int process_json_entries(struct eodhistoricaldata *e, json_value *value)
{
  for(int i = 0; i < value->u.object.length; i++){
    struct balance_sheet_n3 *n3 = NULL;
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;


    PR_DBG("%s: %s\n", e->statement, name);
    process_json_balance_sheet_n3(e, values);
  }
  
  return 0;
}

static int process_json_object(struct eodhistoricaldata *e, json_value *value,
			       const char *parent)
{
  char json_node[256];
  static struct balance_sheet_n3 *n3;

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

static struct balance_sheet_n3 *
eodhistoricaldata_read(struct balance_sheet *ctx)
{
  struct eodhistoricaldata *e = ctx->private;
  struct balance_sheet_n3 *n3 = (void*)list_pop(&e->list_balance_sheet_n3s);

  if(!list_is_head(&n3->list))
    return n3;
  
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

  /* What to look for */
  strcpy(e->statement, "Financials:Balance_Sheet:");
  if(ctx->period == QUARTERLY) strcat(e->statement, "quarterly");
  else strcat(e->statement, "yearly");
  
  /* Init e */
  list_head_init(&e->list_balance_sheet_n3s);
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

static void eodhistoricaldata_release(struct balance_sheet *ctx)
{
  struct eodhistoricaldata *e = ctx->private;
  if(e) free(e);
}

struct balance_sheet_ops balance_sheet_eodhistoricaldata_ops = {
  .init = eodhistoricaldata_init,
  .release = eodhistoricaldata_release,
  .read = eodhistoricaldata_read,
};
