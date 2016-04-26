/*
 * Cresus EVO - development_sim.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 05/04/16
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "sim/sim.h"
#include "input/yahoo.h"
#include "engine/cluster.h"
#include "engine/timeline.h"
#include "indicator/mobile.h"
#include "indicator/roc.h"
#include "indicator/jtrend.h"
#include "framework/verbose.h"

#include <string.h>

#define EMA30  1
#define ROC    2
#define JTREND 3

static struct timeline *
timeline_create(const char *filename, const char *name, time_info_t min,
		list_head_t(struct timeline_entry) *ref_index) {

  struct yahoo *yahoo;
  struct timeline *timeline;
  
  struct roc *roc;
  struct mobile *mobile;
  struct jtrend *jtrend;

  /* TODO : Check return values */
  yahoo_alloc(yahoo, filename, min, TIME_MAX); /* load everything */
  timeline_alloc(timeline, name, __input__(yahoo));
  /* Indicators alloc */
  mobile_alloc(mobile, EMA30, MOBILE_EMA, 30, CANDLE_CLOSE);
  roc_alloc(roc, ROC, 1, 2);
  jtrend_alloc(jtrend, JTREND, 1, 2, ref_index);
  /* And insert */
  timeline_add_indicator(timeline, __indicator__(mobile));
  timeline_add_indicator(timeline, __indicator__(roc));
  timeline_add_indicator(timeline, __indicator__(jtrend));
  
  return timeline;
}

static void timeline_destroy(struct timeline *t) {
}

static void timeline_display_info(struct timeline *t) {

  /* FIXME : change interface */
  struct timeline_entry *entry;
  if(timeline_entry_current(t, &entry) != -1){
    struct indicator_entry *ientry;
    struct candle *candle = __timeline_entry_self__(entry);
    /* Indicators management */
    /* This interface is not easy to use. Find something better */
    candle_indicator_for_each(candle, ientry) {
      switch(ientry->iid){
      case EMA30 : PR_WARN("%s EMA30 is %.2f\n", t->name,
			   ((struct mobile_entry*)
			    __indicator_entry_self__(ientry))->value);
	break;
	
      case ROC :{
	struct roc_entry *e = __indicator_entry_self__(ientry);
	PR_WARN("%s ROC is %.2f, %.2f\n", t->name, e->value, e->average); }
	break;
	
      case JTREND : {
	struct jtrend_entry *e = __indicator_entry_self__(ientry);
	PR_WARN("%s JTREND is %.2f, %.2f\n", t->name, e->value, e->average); }
	break;
      }
    }
  }
}

/* sim interface */

static int sim_feed(struct sim *s, struct cluster *c) {

  struct timeline *t;
  struct timeline_entry *entry;
  
  __slist_for_each__(&c->slist_timeline, t){
    timeline_display_info(t);
    
    if(timeline_entry_current(t, &entry) != -1){
      struct candle *candle = __timeline_entry_self__(entry);
      struct indicator_entry *ientry = 
	candle_find_indicator_entry(candle, JTREND);

      if(ientry) {
	PR_INFO("");
      }
    }
  }
  
  return 0;
}

int main(int argc, char **argv) {

  struct sim sim;
  struct cluster cluster;
  struct timeline *t0, *t1;

  if(argc > 1 && !strcmp(argv[1], "-v"))
    VERBOSE_LEVEL(DBG);

  /* 01/01/2000 */
  time_info_t time = TIME_INIT(2000, 1, 1, 0, 0, 0, 0);
  cluster_init(&cluster, "my cluster", time, TIME_MAX);
  t0 = timeline_create("data/AF.yahoo", "AF", time,
		       &__timeline__(&cluster)->list_entry); /* FIXME */
  t1 = timeline_create("data/AIR.yahoo", "AIR", time,
		       &__timeline__(&cluster)->list_entry);
  
  cluster_add_timeline(&cluster, t0);
  cluster_add_timeline(&cluster, t1);

  /* Now create sim */
  sim_init(&sim, &cluster);
  sim_run(&sim, sim_feed);
  sim_display_report(&sim);
  
  return 0;
}
