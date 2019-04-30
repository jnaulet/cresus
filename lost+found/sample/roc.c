/*
 * Cresus EVO - roc.c
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
#include "engine/engine.h"
#include "indicator/roc.h"

#include "framework/verbose.h"

#define ROC 1
#define ROC_PERIOD   12

static void run_timeline_roc(struct timeline *t,
			     struct candle *c,
			     struct roc_n3 *re)
{
  printf("%.1f ", re->value);
}

static int feed(struct engine *e,
		struct timeline *t,
		struct timeline_n3 *n3)
{
  /* Step by step loop */
  static int n = 0;
  struct indicator_n3 *in3;
  struct candle *c = __timeline_n3_self__(n3);
  /* Execute */
  PR_INFO("%s - ", candle_str(c));
  /* Then check results */
  __slist_for_each__(&c->slist_indicator, in3){
    /* Beware, some parsing will be required here to determine who's who */
    printf("%s(%u) ", in3->indicator->str, in3->indicator->id);
    switch(in3->indicator->id){
    case ROC : {
      struct roc_n3 *re = __indicator_n3_self__(in3);
      run_timeline_roc(t, c, re);
      goto next;
    }}
  next:
    n++;
  }
  
  printf("- %d\n", n);
  return 0;
}

static struct timeline *timeline_create(const char *filename)
{
  struct yahoo *yahoo;
  struct timeline *timeline;
  
  if(yahoo_alloc(yahoo, filename, TIME64_MIN, TIME64_MAX)){
    if(timeline_alloc(timeline, "roc", __input__(yahoo))){
      struct roc *roc;
      /* Add a ROC indicator */
      roc_alloc(roc, ROC, ROC_PERIOD, 1);
      timeline_add_indicator(timeline, __indicator__(roc));
      return timeline;
    }
  }

  return NULL;
}

int main(int argc, char **argv) {

  VERBOSE_LEVEL(INFO);
  
  /*
   * Data
   */
  struct timeline *t;
  struct engine engine;

  if(argc != 2){
    fprintf(stdout, "Usage: %s filename.yahoo\n", argv[0]);
    return -1;
  }
  
  if((t = timeline_create(argv[1]))){
    engine_init(&engine, t);
    engine_run(&engine, feed);
    /* Release */
    engine_release(&engine);
  }

  return 0;
}
