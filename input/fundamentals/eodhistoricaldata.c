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

#include "framework/time.h"
#include "framework/fundamentals.h"

struct eodhistoricaldata {
  char statement[256];
  list_head_t(struct balance_sheet_n3) list_balance_sheet_n3s_q;
  list_head_t(struct balance_sheet_n3) list_balance_sheet_n3s_y;
  list_head_t(struct income_statement_n3) list_income_statement_n3s_q;
  list_head_t(struct income_statement_n3) list_income_statement_n3s_y;
  list_head_t(struct cash_flow_n3) list_cash_flow_n3s_q;
  list_head_t(struct cash_flow_n3) list_cash_flow_n3s_y;
};

/*
 * Misc
 */

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
  if(value->u.string.ptr) return iso8601_to_time(value->u.string.ptr);
  else return 0;
}

static long long json_str_to_int(json_value *value)
{
  if(value->u.string.ptr)
    return strtoll(value->u.string.ptr, NULL, 10);
  
  return 0;
}

/*
 * Balance sheet
 */

static int process_json_balance_sheet_n3(struct eodhistoricaldata *e,
					 json_value *value,
                                         list_head_t *list)
{
  struct balance_sheet_n3 *n3 = NULL;
  
  for(int i = 0; i < value->u.object.length; i++){
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;
    
    PR_DBG("DATA: %s\n", name);
    
    /* Create object */
    if(!strcasecmp("date", name))
      n3 = balance_sheet_n3_alloc(n3, iso8601_to_time(values->u.string.ptr));

    /* Real date might be different (+1 month probably) */
    if(!strcasecmp("filing_date", name) && values->u.string.ptr)
      n3->time = json_str_to_time(values);

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
  if(n3) list_add(list, &n3->list);
  return 1;
}

/*
 * Income statement
 */

static int process_json_income_statement_n3(struct eodhistoricaldata *e,
                                            json_value *value,
                                            list_head_t *list)
{
  struct income_statement_n3 *n3 = NULL;
  
  for(int i = 0; i < value->u.object.length; i++){
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;
    
    PR_DBG("DATA: %s\n", name);
    
    /* Create object */
    if(!strcasecmp("date", name))
      n3 = income_statement_n3_alloc(n3, iso8601_to_time(values->u.string.ptr));

    /* Real date might be different (+1 month probably) */
    if(!strcasecmp("filing_date", name) && values->u.string.ptr)
      n3->time = json_str_to_time(values);

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
  if(n3) list_add(list, &n3->list);
  return 1;
}

/*
 * Cash flow
 */

static int process_json_cash_flow_n3(struct eodhistoricaldata *e,
                                     json_value *value,
                                     list_head_t *list)
{
  struct cash_flow_n3 *n3 = NULL;
  
  for(int i = 0; i < value->u.object.length; i++){
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;
    
    PR_DBG("DATA: %s\n", name);
    
    /* Create object */
    if(!strcasecmp("date", name))
      n3 = cash_flow_n3_alloc(n3, iso8601_to_time(values->u.string.ptr));

    /* Real date might be different (+1 month probably) */
    if(!strcasecmp("filing_date", name) && values->u.string.ptr)
      n3->time = json_str_to_time(values);
    
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
  if(n3) list_add(list, &n3->list);
  return 1;
}

typedef int (*process_n3s_ptr)(struct eodhistoricaldata*, json_value*, list_head_t*);

static int process_json_n3(struct eodhistoricaldata *e, json_value *value,
                           list_head_t *list, process_n3s_ptr fn)
{
  for(int i = 0; i < value->u.object.length; i++){
    struct cash_flow_n3 *n3 = NULL;
    char *name = value->u.object.values[i].name;
    json_value *values = value->u.object.values[i].value;
    if(fn) fn(e, values, list);
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

    /* SharesStats */
    if(!strcasecmp(json_node, "SharesStats:SharesOutstanding"))
      PR_ERR("Found SharesStats:SharesOutstanding !!!\n");
    
    /* Balance sheet */
    if(!strcasecmp(json_node, "Financials:Balance_Sheet:quarterly"))
      return process_json_n3(e, values, &e->balance_sheet_n3s_q, process_json_balance_sheet_n3);
    if(!strcasecmp(json_node, "Financials:Balance_Sheet:yearly"))
      return process_json_n3(e, values, &e->balance_sheet_n3s_y, process_json_balance_sheet_n3);
    /* Income statement */
    if(!strcasecmp(json_node, "Financials:Income_Statement:quarterly"))
      return process_json_n3(e, values, &e->income_statement_n3s_q, process_json_income_statement_n3);
    if(!strcasecmp(json_node, "Financials:Income_Statement:yearly"))
      return process_json_n3(e, values, &e->income_statement_n3s_y, process_json_income_statement_n3);
    /* Cash flow */
    if(!strcasecmp(json_node, "Financials:Cash_Flow:quarterly"))
      return process_json_n3(e, values, &e->cazsh_flow_n3s_q, process_json_cash_flow_n3);
    if(!strcasecmp(json_node, "Financials:Cash_Flow:yearly"))
      return process_json_n3(e, values, &e->cazsh_flow_n3s_y, process_json_cash_flow_n3);
    
    /* Round trip */
    if(process_json_object(e, values, json_node) < 0){
      return -1;
    }
  }
  
  return 0;
}

/*
 * All the read functions in one macro
 */

#define READ(func, list, type)			\
  type *func(struct fundamentals *ctx){		\
    struct eodhistoricaldata *e = ctx->private;	\
    type *n3 = (void*)list_pop(&e->list);	\
    if(!list_is_head(&n3->list)) return n3;	\
    return NULL;				\
  }

READ(eodhistoricaldata_read_bq, list_balance_sheet_n3s_q, struct balance_sheet_n3);
READ(eodhistoricaldata_read_by, list_balance_sheet_n3s_y, struct balance_sheet_n3);
READ(eodhistoricaldata_read_iq, list_income_statement_n3s_q, struct income_statement_n3);
READ(eodhistoricaldata_read_iy, list_income_statement_n3s_y, struct income_statement_n3);
READ(eodhistoricaldata_read_cq, list_cash_flow_n3s_q, struct cash_flow_n3);
READ(eodhistoricaldata_read_cy, list_cash_flow_n3s_y, struct cash_flow_n3);

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
  list_head_init(&e->list_balance_sheet_n3s_q);
  list_head_init(&e->list_balance_sheet_n3s_y);
  list_head_init(&e->list_income_statement_n3s_q);
  list_head_init(&e->list_income_statement_n3s_y);
  list_head_init(&e->list_cash_flow_n3s_q);
  list_head_init(&e->list_cash_flow_n3s_y);
  
  /* What to look for */
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

struct fundamentals_ops fundamentals_eodhistoricaldata_ops = {
  .init = eodhistoricaldata_init,
  .release = eodhistoricaldata_release,
  .read_bq = eodhistoricaldata_read_bq,
  .read_by = eodhistoricaldata_read_by,
  .read_iq = eodhistoricaldata_read_iq,
  .read_iy = eodhistoricaldata_read_iy,
  .read_cq = eodhistoricaldata_read_cq,
  .read_cy = eodhistoricaldata_read_cy,
};
