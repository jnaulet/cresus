#include <stdio.h>

#include "technical/rsi.h"
#include "technical/atr.h"
#include "technical/macd.h"
#include "parser/yahoo.h"
#include "core/candle.h"
#include "core/statistics.h"

#define FILE "../csv/px1-historical.csv"

#define RSI_P       12
#define ATR_P       14
#define MACD_FAST_P 10
#define MACD_SLOW_P 20
#define MACD_SIG_P   9

int main(int argc, char **argv)
{
  struct yahoo y;
  struct candle cdl;

  /* indicators */
  struct rsi rsi;
  struct atr atr;
  struct macd macd;
  
  if(yahoo_init(&y, FILE) != -1 &&
     yahoo_read(&y, &cdl)){

    /* Init */
    atr_init(&atr, ATR_P, &cdl);
    rsi_init(&rsi, RSI_P, &cdl);
    macd_init(&macd, MACD_FAST_P, MACD_SLOW_P, MACD_SIG_P, &cdl);
    
  }else /* Can't open file */
    return -1;
  
  for(;;){
    
    char buf[256];
    int day, month, year;

    if(!fgets(buf, sizeof buf, stdin))
      break;

    /* Scan input */
    sscanf(buf, "%d-%d-%d", &year, &month, &day);

    for(;;){
      
      int out = 0;

      /* We get next day data */
      if(!yahoo_read(&y, &cdl))
	break;
      
      if(cdl.day == day &&
	 cdl.month == month &&
	 cdl.year == year){

	struct macd_result *res = macd_compute(&macd);
	printf("%02d-%02d-%04d : " \
	       "rsi %.2lf, atr %.2lf, macd %+.2lf %+.2lf (%+.2lf)\n",
	       day, month, year,
	       rsi_compute(&rsi), atr_compute(&atr),
	       res->value, res->signal, res->histogram);

	out = 1;
      }

      /* After we got all data (EOD) */
      rsi_feed(&rsi, &cdl);
      atr_feed(&atr, &cdl);
      macd_feed(&macd, &cdl);

      if(out)
	break;
    }
  }
  
  /* Free all */
  yahoo_free(&y);
  rsi_free(&rsi);
  atr_free(&atr);
  macd_free(&macd);

  return 0;
}
