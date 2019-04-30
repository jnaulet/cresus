/*
 * Cresus EVO - basics.c
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

#include "input/inwrap.h"
#include "engine/engine.h"
#include "indicator/macd.h"
#include "indicator/mobile.h"
#include "indicator/rs_dorsey.h"
#include "indicator/rs_mansfield.h"

#include "framework/verbose.h"

#define EMA40 0
#define EMA14 1
#define EMA5  2
#define MACD  3
#define RSM   4
#define RSD   5

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
    case EMA40 :
    case EMA14:
    case EMA5 : {
      struct mobile_n3 *mob = __indicator_n3_self__(in3);
      printf("%.1f ", mob->value);
      goto next;
    }
    case RSM : {
      struct rs_mansfield_n3 *rs = __indicator_n3_self__(in3);
      printf("%.1f ", rs->value);
      goto next;
    }
    case RSD : {
      struct rs_dorsey_n3 *rs = __indicator_n3_self__(in3);
      printf("%.1f ", rs->value);
    } goto next;
    }
  next:
    n++;
  }
  
  printf("- %d\n", n);
  return 0;
}

static struct timeline *timeline_create(const char *filename, const char *type)
{
  /*
   * Data
   */
  struct inwrap *inwrap;
  struct timeline *timeline;
  inwrap_t t = inwrap_t_from_str(type);
  
  if(inwrap_alloc(inwrap, filename, t, TIME64_MIN, TIME64_MAX)){
    if(timeline_alloc(timeline, "basics", __input__(inwrap))){
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
      rs_mansfield_alloc(rsm, RSM, 14, &timeline->list_n3);
      timeline_add_indicator(timeline, __indicator__(rsm));
      /* RS Dorsey */
      struct rs_dorsey *rsd;
      rs_dorsey_alloc(rsd, RSD, &timeline->list_n3);
      timeline_add_indicator(timeline, __indicator__(rsd));
      /* Ok */
      return timeline;
    }
  }

  return NULL;
}

int main(int argc, char **argv)
{
  VERBOSE_LEVEL(INFO);
  
  /*
   * Data
   */
  int c;
  char *filename, *type;
  
  struct timeline *t;
  struct engine engine;

  if(argc < 2){
    fprintf(stdout, "Usage: %s -o type filename\n", argv[0]);
    return -1;
  }

  while((c = getopt(argc, argv, "o:")) != -1){
    switch(c){
    case 'o': type = optarg; break;
    default:
      PR_ERR("Unknown option %c\n", c);
      return -1;
    }
  }

  /* Filename is only real param */
  filename = argv[optind];
  
  if((t = timeline_create(filename, type))){
    engine_init(&engine, t);
    engine_run(&engine, feed);
    /* TODO : Don't forget to release everything */
    engine_release(&engine);
  }
  
  return 0;
}
