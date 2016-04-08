/*
 * Cresus EVO - cresus_rsi.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>

#include "rsi.h"
#include "atr.h"
#include "macd.h"
#include "yahoo.h"
#include "candle.h"
#include "statistics.h"

#define FILE "../csv/px1-historical.csv"

#define RSI_P 12
#define RSI_HI 70.0 /* Ceil up */
#define RSI_LO 35.0 /* Ceil down */

#define ATR_P 14

#define MACD_FAST_P 12
#define MACD_SLOW_P 26
#define MACD_SIG_P   9

#define OP_COST  5.0
#define STOPLOSS 500.0

int main(int argc, char **argv)
{
  struct yahoo y;
  struct rsi rsi;
  struct atr atr;
  struct candle cdl;
  struct statistics stats;

  /* Macd */
  struct macd macd;
  double cur_hist = 0.0, last_hist = 0.0;

  /* State machine */
  int state = 0;
  double buy, val;

  if(yahoo_init(&y, FILE) != -1 &&
     yahoo_read(&y, &cdl)){
    /* Init */
    atr_init(&atr, ATR_P, &cdl);
    rsi_init(&rsi, RSI_P, &cdl);
    macd_init(&macd, MACD_FAST_P, MACD_SLOW_P, MACD_SIG_P, &cdl);
    /* Statistics */
    statistics_init(&stats, OP_COST);

  }else /* Can't open file */
    return -1;
  
  for(;;){
    
    /* We get next day data */
    if(!yahoo_read(&y, &cdl))
      break;

    /* Before we get any data (SOD) */
    if(!(val = rsi_compute(&rsi)))
      goto next;

    /* printf("Candle %.2lf rsi %.2lf\n", cdl.close, val); */
    
    switch(state){
    case 0 : /* Look for entry point */
      /* Memorize open value */
      buy = cdl.open;

      /* if(val <= RSI_LO){ */
      /* 	state = 1; */
      /* 	printf("L for %.2lf at %s\n", cdl.open, cdl.date); */
      /* } */
      if(last_hist >= 0 && cur_hist < 0){
	state = 2;
	printf("S for %.2lf at %q\n", cdl.open, candle_localtime_str(&cdl));
	printf("RSI %.2lf ATR %.2lf\n", val, atr_compute(&atr));
      }
      break;

    case 1 : /* Long */
      if(val >= RSI_HI || cdl.close < (buy - STOPLOSS)){
	statistics_accumulate(&stats, (cdl.close - buy));
	state = 0;
	
	printf("R for %.2lf (%+.2lf) at %s\n", cdl.close,
	       (cdl.close - buy), candle_localtime_str(&cdl));
      }
      break;
      
    case 2 : /* Short */
      if(val <= RSI_LO || cdl.close > (buy + STOPLOSS)){
	statistics_accumulate(&stats, (buy - cdl.close));
	state = 0;
	
	printf("C for %.2lf (%+.2lf) at %s\n", cdl.close,
	       (buy - cdl.close), candle_localtime_str(&cdl));
      }
      break;
    }

  next:
    /* After we got all data (EOD) */
    rsi_feed(&rsi, &cdl);
    atr_feed(&atr, &cdl);
    last_hist = macd_compute(&macd)->histogram;
    cur_hist = macd_feed(&macd, &cdl)->histogram;
  }
  
  statistics_printf(&stats);

  /* Free all */
  yahoo_release(&y);
  rsi_release(&rsi);
  statistics_release(&stats);

  return 0;
}
