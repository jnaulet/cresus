#include <stdio.h>

#include "atr.h"
#include "esi.h"
#include "rsi.h"
#include "macd.h"
#include "yahoo.h"
#include "candle.h"
#include "zigzag.h"
#include "bollinger.h"
#include "statistics.h"

#define FILE "../csv/px1-2011-2013.csv"

#define RSI_P      12
#define ATR_P      14

/* ESI */
#define ESI_P     6
#define MOBILE_P 40

/* ZZ */
#define ZIGZAG_THRES  4.0
#define ZIGZAG_TYPE   ZIGZAG_PERCENT

/* MACD */
#define MACD_FAST_P 10
#define MACD_SLOW_P 20
#define MACD_SIG_P   9

/* Bollinger */
#define BOLLINGER_P   14
#define BOLLINGER_DEV  2

int main(int argc, char **argv)
{
  struct yahoo y;
  struct candle cdl;

  /* indicators */
  struct atr atr;
  struct esi esi;
  struct rsi rsi;
  struct macd macd;
  struct mobile ma;
  struct zigzag zz;
  struct bollinger boll;

  if(yahoo_init(&y, FILE) != -1 &&
     yahoo_read(&y, &cdl)){

    /* Mobile & its ESI */
    mobile_init(&ma, MOBILE_MMA, MOBILE_P, cdl.close);
    esi_init(&esi, MOBILE_EMA, ESI_P);
    
    /* Init */
    atr_init(&atr, ATR_P, &cdl);
    rsi_init(&rsi, RSI_P, &cdl);

    macd_init(&macd, MACD_FAST_P, MACD_SLOW_P, MACD_SIG_P, &cdl);
    bollinger_init(&boll, BOLLINGER_P, BOLLINGER_DEV, &cdl);
    zigzag_init(&zz, ZIGZAG_TYPE, ZIGZAG_THRES, &cdl);
    
  }else /* Can't open file */
    return -1;
  
  for(;;){

    struct macd_result *mr = macd_compute(&macd);
    struct bollinger_res *br = bollinger_compute(&boll);
    
#define BOLLINGER_CEIL 5

    if(br && (cdl.close - br->lo) <= -BOLLINGER_CEIL && /* Bollinger */
       zigzag_compute(&zz) == ZIGZAG_DIR_DOWN && !zz.ref_count && /* Zigzag */
       mr->value < 0 /* MACD */) { /* TODO : Use another indicator */

      /* Entry point found */
      printf("%s - %.2lf - ", /* Basic */
	     candle_localtime_str(&cdl), cdl.close);
      printf("rsi(%d) %.2lf, ", RSI_P, rsi_compute(&rsi)); /* RSI */
      printf("esi(%d,%d) %.2lf, ", MOBILE_P, ESI_P, esi_compute(&esi));
      printf("atr(%d) %.2lf\n", ATR_P, atr_compute(&atr)); /* ATR */
      printf("macd(%d,%d,%d) %+.2lf %+.2lf (%+.2lf)\n", /* MACD */
	     MACD_FAST_P, MACD_SLOW_P, MACD_SIG_P,
	     mr->value, mr->signal, mr->histogram);
      printf("boll(%d,%d) %.2lf %.2lf %.2lf\n",  /* Boll */
	     BOLLINGER_P, BOLLINGER_DEV, br->mma, br->hi, br->lo);
      printf("np boll lo = %.2lf\n", cdl.close - br->lo);

      printf("\n"); /* Jump line */
    }

    /* We get next day data */
    if(!yahoo_read(&y, &cdl))
      break;
    
    /* Feed MMA40 & its direction */
    esi_feed(&esi, mobile_feed(&ma, cdl.close));

    /* After we got all data (EOD) */
    atr_feed(&atr, &cdl);
    rsi_feed(&rsi, &cdl);
    macd_feed(&macd, &cdl);
    bollinger_feed(&boll, &cdl);
    zigzag_feed(&zz, &cdl);
  }
  
  /* Free all */
  yahoo_free(&y);
  rsi_free(&rsi);
  esi_free(&esi);
  atr_free(&atr);
  macd_free(&macd);

  return 0;
}
