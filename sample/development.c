/*
 * Cresus EVO - development.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 07/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 *
 * Development sample code
 * Used to determine if interfaces are coherent for the "user"
 * And maybe to implement some kind of script commands 
 *
 */

#include <stdio.h>
#include <getopt.h>

#include "input/yahoo.h"
#include "engine/timeline.h"
#include "indicator/macd.h"
#include "indicator/mobile.h"
#include "indicator/rs_dorsey.h"
#include "indicator/rs_mansfield.h"

#define EMA40 0
#define EMA14 1
#define EMA5  2
#define MACD  3
#define RSM   4
#define RSD   5

static void run_timeline(struct timeline *t) {

  /* Step by step loop */
  struct timeline_entry *entry;
  while(timeline_entry_next(t, &entry) != -1){
    int n = 0;
    struct indicator_entry *ientry;
    struct candle *c = __timeline_entry_self__(entry);
    /* Execute */
    timeline_step(t, entry);
    printf("%s - ", candle_str(__timeline_entry_self__(entry)));
    /* Then check results */
    __slist_for_each__(&c->slist_indicator, ientry){
      /* Beware, some parsing will be required here to determine who's who */
      printf("%s(%u) ", ientry->indicator->str, ientry->indicator->id);
      switch(ientry->indicator->id){
      case EMA40 :
      case EMA14:
      case EMA5 : {
	struct mobile_entry *mob = __indicator_entry_self__(ientry);
	printf("%.1f ", mob->value);
	goto next;
      }
      case RSM : {
	struct rs_mansfield_entry *rs = __indicator_entry_self__(ientry);
	printf("%.1f ", rs->value);
	goto next;
      }
      case RSD : {
	struct rs_dorsey_entry *rs = __indicator_entry_self__(ientry);
	printf("%.1f ", rs->value);
      } goto next;
      }
    next:
      n++;
    }
    
    printf("- %d\n", n);
  }
}

int main(int argc, char **argv) {

  /*
   * Data
   */
  struct yahoo *yahoo;
  struct timeline *timeline;

  if(yahoo_alloc(yahoo, "data/FCHI.yahoo", TIME_MIN, TIME_MAX)){
    if(timeline_alloc(timeline, "^FCHI", __input__(yahoo))){
      struct mobile *m;
      /* Add a series of EMAs */
      mobile_alloc(m, EMA40, MOBILE_EMA, 40, CANDLE_CLOSE);
      timeline_add_indicator(timeline, __indicator__(m));
      mobile_alloc(m, EMA14, MOBILE_EMA, 14, CANDLE_CLOSE);
      timeline_add_indicator(timeline, __indicator__(m));
      mobile_alloc(m, EMA5, MOBILE_EMA, 5, CANDLE_CLOSE);
      timeline_add_indicator(timeline, __indicator__(m));
      /* Macd */
      struct macd *macd;
      macd_alloc(macd, MACD, 12, 26, 9);
      timeline_add_indicator(timeline, __indicator__(macd));
      /* RS mansfield */
      struct rs_mansfield *rsm;
      rs_mansfield_alloc(rsm, RSM, 14, &timeline->list_entry);
      timeline_add_indicator(timeline, __indicator__(rsm));
      /* RS Dorsey */
      struct rs_dorsey *rsd;
      rs_dorsey_alloc(rsd, RSD, &timeline->list_entry);
      timeline_add_indicator(timeline, __indicator__(rsd));

      /* Execute all this */
      run_timeline(timeline);
    }
  }
  
  /* TODO : Don't forget to release everything */
  
  return 0;
}
