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
#include "indicator/roc.h"

#include "framework/verbose.h"

#define ROC 1
#define ROC_PERIOD   12

static void run_timeline_roc(struct timeline *t,
			     struct candle *c,
			     struct roc_entry *re)
{
  printf("%.1f ", re->value);
}

static void run_timeline(struct timeline *t) {

  /* Step by step loop */
  char buf[256]; /* debug */
  struct timeline_entry *entry;
  while(timeline_entry_next(t, &entry) != -1){
    int n = 0;
    struct indicator_entry *ientry;
    struct candle *c = __timeline_entry_self__(entry);
    /* Execute */
    timeline_append_entry(t, entry);
    timeline_step(t);
    printf("%s - ", candle_str(__timeline_entry_self__(entry), buf));
    /* Then check results */
    __slist_for_each__(&c->slist_indicator, ientry){
      /* Beware, some parsing will be required here to determine who's who */
      printf("%s(%u) ", ientry->indicator->str, ientry->indicator->id);
      switch(ientry->indicator->id){
      case ROC : {
	struct roc_entry *re = __indicator_entry_self__(ientry);
	run_timeline_roc(t, c, re);
	goto next;
      }}
    next:
      n++;
    }
    
    printf("- %d\n", n);
  }
}

int main(int argc, char **argv) {

  VERBOSE_LEVEL(DBG);
  
  /*
   * Data
   */
  struct yahoo *yahoo;
  struct timeline *timeline;

  if(yahoo_alloc(yahoo, "data/%5EFCHI.yahoo", TIME_MIN, TIME_MAX)){
    if(timeline_alloc(timeline, "^FCHI", __input__(yahoo))){
      struct roc *roc;
      /* Add a ROC indicator */
      roc_alloc(roc, ROC, ROC_PERIOD, 1);
      timeline_add_indicator(timeline, __indicator__(roc));
      /* Execute all this */
      run_timeline(timeline);
      
      /* Free */
      roc_free(roc);
      timeline_free(timeline);
    }
  }
  
  return 0;
}
