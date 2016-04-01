/*
 * Cresus EVO - cresus_atr.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <float.h>

#include "atr.h"
#include "yahoo.h"
#include "candle.h"
#include "zigzag.h"
#include "portfolio.h"
#include "statistics.h"

#define FILE "../csv/px1-2011-2013.csv"

/* ATR */
#define ATR_P      14
#define ATR_FACTOR 1.0
/* ZZ */
#define ZIGZAG_THRES  4.0
#define ZIGZAG_TYPE   ZIGZAG_PERCENT
/* STATS */
#define OP_COST 5.0 /* Euros */

int main(int argc, char **argv)
{
  struct yahoo y;
  struct candle cdl;

  /* indicators */
  struct atr atr;
  struct zigzag zz;
  struct portfolio pf;
  struct statistics stats;

  enum {
    BUY,
    SELL
  } state = BUY;

  /* Misc */
  double rpoint = DBL_MAX, stop = 0;
  
  if(yahoo_init(&y, FILE) != -1 &&
     yahoo_read(&y, &cdl)){

    /* Init */
    atr_init(&atr, ATR_P, &cdl);
    zigzag_init(&zz, ZIGZAG_TYPE, ZIGZAG_THRES, &cdl);

    portfolio_init(&pf);
    statistics_init(&stats, OP_COST);
    
  }else /* Can't open file */
    return -1;

  for(;;){

    switch(state){
    case BUY :
      /* Potential entry point found */
      if(zigzag_compute(&zz) == ZIGZAG_DIR_DOWN && !zz.ref_count){
	/* Set resistance point */
	rpoint = cdl.high + atr_compute(&atr) / ATR_FACTOR;

	printf("rpoint %s - %.2lf\n",
	       candle_localtime_str(&cdl),
	       rpoint);
	}
    
      if(cdl.close > rpoint){

	/* Correct value if necessary */
	//rpoint = (cdl.open > rpoint ? cdl.open : rpoint);
	portfolio_buy(&pf, PORTFOLIO_LONG, cdl.close);

	/* Set moving stop */
	stop = cdl.high - atr_compute(&atr) / ATR_FACTOR;
	state = SELL;

	printf("L %s - %.2lf (%.2lf)\n",
	       candle_localtime_str(&cdl),
	       rpoint, stop);
      }
      break;

    case SELL :
      if(cdl.close < stop){
	double value;

	//stop = (cdl.open < stop ? cdl.open : stop);
	value = portfolio_sell(&pf, cdl.close);

	/* Go back to buy mode */
	rpoint = DBL_MAX;
	state = BUY;
	
	statistics_accumulate(&stats, value);

	printf("R %s - %.2lf (%+.2lf)\n",
	       candle_localtime_str(&cdl),
	       stop, value);
      }

      if(zigzag_compute(&zz) == ZIGZAG_DIR_UP && !zz.ref_count){
	/* Set moving stop */
	stop = cdl.close - atr_compute(&atr) / ATR_FACTOR;

	printf("stop %s - %.2lf\n",
	       candle_localtime_str(&cdl),
	       stop);
      }

      /* stop = (cdl.close - atr_compute(&atr) > stop ? */
      /* 	      cdl.close - atr_compute(&atr) : stop); */

      /* printf("stop = %.2lf\n", stop); */
    }

    /* We get next day data */
    if(!yahoo_read(&y, &cdl))
      break;
    
    /* After we got all data (EOD) */
    atr_feed(&atr, &cdl);
    zigzag_feed(&zz, &cdl);
  }
  
  statistics_printf(&stats);

  /* Free all */
  yahoo_free(&y);
  atr_free(&atr);
  zigzag_free(&zz);
  portfolio_free(&pf);
  statistics_free(&stats);

  return 0;
}
