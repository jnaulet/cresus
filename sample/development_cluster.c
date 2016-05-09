/*
 * Cresus EVO - development_cluster.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/24/16
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "input/yahoo.h"
#include "engine/cluster.h"
#include "engine/timeline.h"
#include "indicator/mobile.h"
#include "indicator/rs_mansfield.h"

#include "framework/verbose.h"

#include <string.h>

#define EMA30 1
#define RSM   2

static struct timeline *
timeline_ref_create(const char *filename, const char *name, time_info_t min) {
  
  struct yahoo *yahoo;
  struct timeline *timeline;
  
  /* TODO : check */
  yahoo_alloc(yahoo, filename, min, TIME_MAX);
  timeline_alloc(timeline, name, __input__(yahoo));

  return timeline;
}

static struct timeline *
timeline_create(const char *filename, const char *name, time_info_t min,
		list_head_t(struct timeline_entry) *ref_index) {

  struct yahoo *yahoo;
  struct timeline *timeline;
  
  struct mobile *mobile;
  struct rs_mansfield *rsm;

  /* TODO : Check return values */
  yahoo_alloc(yahoo, filename, min, TIME_MAX); /* load everything */
  timeline_alloc(timeline, name, __input__(yahoo));
  /* Indicators alloc */
  mobile_alloc(mobile, EMA30, MOBILE_EMA, 30, CANDLE_CLOSE);
  rs_mansfield_alloc(rsm, RSM, 14, ref_index);
  /* And insert */
  timeline_add_indicator(timeline, __indicator__(mobile));
  timeline_add_indicator(timeline, __indicator__(rsm));
  
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
	
      case RSM : PR_WARN("%s RSM is %.2f\n", t->name,
			 ((struct rs_mansfield_entry*)
			  __indicator_entry_self__(ientry))->value);
	break;
      }
    }
  }
}

int main(int argc, char **argv) {

  struct cluster cluster;
  struct timeline *t0, *t1, *t2;

  if(argc > 1 && !strcmp(argv[1], "-v"))
    VERBOSE_LEVEL(DBG);

  /* 01/01/2000 */
  time_info_t time = TIME_INIT(2000, 1, 1, 0, 0, 0, 0);
  cluster_init(&cluster, "my cluster", NULL, time, TIME_MAX);
  t0 = timeline_ref_create("data/%5EFCHI.yahoo", "^FCHI", time);
  t1 = timeline_create("data/AF.yahoo", "AF", time, &t0->list_entry);
  t2 = timeline_create("data/AIR.yahoo", "AIR", time, &t0->list_entry);

  cluster_add_timeline(&cluster, t0);
  cluster_add_timeline(&cluster, t1);
  cluster_add_timeline(&cluster, t2);
  
  /* And then ? */
  while(cluster_step(&cluster) != -1){
    timeline_display_info(t0);
    timeline_display_info(t1);
    timeline_display_info(t2);
  }

  return 0;
}
