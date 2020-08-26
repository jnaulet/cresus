#include "sim/sim.h"
#include "input/yahoo.h"
#include "engine/cluster.h"
#include "engine/timeline.h"
#include "indicator/zigzag.h"
#include "framework/verbose.h"

#include <getopt.h>
#include <string.h>

#define ZIGZAG 1
#define START_TIME VAL_YEAR(2012)|VAL_MONTH(1)|VAL_DAY(1)

#define ZIGZAG_THRES 0.03 /* 1/100th */
#define ZIGZAG_WINDOW 10

static double zz_thres = ZIGZAG_THRES;
static int zz_window = ZIGZAG_WINDOW;

static struct timeline *
timeline_create(const char *filename, const char *name, time_t min) {

  struct yahoo *yahoo;
  struct zigzag *zigzag;
  struct timeline *timeline;
  
  /* TODO : Check return values */
  yahoo_alloc(yahoo, filename, START_TIME, TIME64_MAX); /* load everything */
  timeline_alloc(timeline, name, __input__(yahoo));
  /* Indicators alloc */
  zigzag_alloc(zigzag, ZIGZAG, zz_thres, CANDLE_CLOSE);
  /* And insert */
  timeline_add_indicator(timeline, __indicator__(zigzag));
  
  return timeline;
}

/* more final functions */

static void add_timeline_to_cluster(struct cluster *c, const char *path,
				    const char *name, time_t time) {
  
  struct timeline *sub;
  sub = timeline_create(path, name, time);
  cluster_add_timeline(c, sub);
}

/* Global */

static zigzag_dir_t __trend__;

static int trend_set(zigzag_dir_t trend) {
  
  zigzag_dir_t old = __trend__;
  __trend__ = trend;
  /* Choose what's best here */
  return (__trend__ != old);
}

/* sim interface */

static int sim_feed(struct sim *s, struct cluster *c) {

  char buf[256];
  int status = 0;
  
  struct timeline *t;
  struct timeline_n3 *n3;
  struct indicator_n3 *in3;
  struct zigzag_n3 *zref = NULL;
  
  /* TODO : better management of this ? */
  if(timeline_n3_current(__timeline__(c), &n3) != -1){
    struct candle *candle = __timeline_n3_self__(n3);
    if((in3 = candle_find_indicator_n3(candle, ZIGZAG))){
      zref = __indicator_n3_self__(in3);
      PR_WARN("%s ZIGZAG is %.2f\n", __timeline__(c)->name, zref->value);
      /* Manage cluster's status here */
      if(trend_set(zref->dir)){
	/* We got a change here */
	PR_INFO("General trend switched to %s\n",
		__trend__ == ZIGZAG_UP ? "up" : "down");
	/* Close all positions */
	sim_close_all_positions(s);
      }
    }
  }
  
  __slist_for_each__(&c->slist_timeline, t){
    /* TODO : this is facultative */
    //timeline_display_info(t);
    /* This is mandatory */
    if(t->status == TIMELINE_STATUS_RESET){
      /* Timeline's been reset. We gotta leave our positions */
      sim_ignore_position(s, t);
      continue;
    }
    
    if(timeline_n3_current(t, &n3) != -1){
      struct position *p;
      struct candle *candle = __timeline_n3_self__(n3);
      if((in3 = candle_find_indicator_n3(candle, ZIGZAG))){
	struct zigzag_n3 *zn3 = __indicator_n3_self__(in3);
	PR_WARN("%s ZZ is %.2f\n", t->name, zn3->value);
	
	/* Always check if something's open */
	sim_find_opened_position(s, t, &p);

	/* LONG positions */
	if(__trend__ == ZIGZAG_UP){
	  if(zn3->value > zref->value &&
	     zref->n_since_last_ref <= zz_window){
	    if(p == NULL){
	      sim_open_position(s, t, POSITION_LONG, 1);
	      PR_ERR("Taking LONG position on %s at %s\n",
		     t->name, candle_str(candle, buf));
	    }
	  }else{
	    if(p != NULL){
	      sim_close_position(s, t);
	      PR_ERR("Leaving LONG position on %s at %s\n",
		     t->name, candle_str(candle, buf));
	    }
	  }
	}

	/* SHORT positions */
	if(__trend__ == ZIGZAG_DOWN){
	  if(zn3->value < zref->value &&
	     zref->n_since_last_ref <= zz_window){
	    if(p == NULL){
	      sim_open_position(s, t, POSITION_SHORT, 1);
	      PR_ERR("Taking SHORT position on %s at %s\n",
		     t->name, candle_str(candle, buf));
	    }
	  }else{
	    if(p != NULL){
	      sim_close_position(s, t);
	      PR_ERR("Leaving SHORT position on %s at %s\n",
		     t->name, candle_str(candle, buf));
	    }
	  }
	}
      }
    }
  }
}

int main(int argc, char **argv) {

  int c;
  struct sim sim;
  struct zigzag zigzag;
  struct cluster cluster;
  
  /* VERBOSE_LEVEL(WARN); */
  
  while((c = getopt(argc, argv, "vp:w:")) != -1){
    switch(c){
    case 'p' : sscanf(optarg, "%lf", &zz_thres); break;
    case 'v' : VERBOSE_LEVEL(DBG); break;
    case 'w' : sscanf(optarg, "%d", &zz_window); break;
    }
  }
  
  /* 01/01/2000 */
  struct yahoo *yahoo;
  time_t time = START_TIME;
  /* Load ref data */
  yahoo_alloc(yahoo, "data/%5EFCHI.yahoo", time, TIME64_MAX);
  cluster_init(&cluster, "my cluster", __input__(yahoo), time, TIME64_MAX);
  /* Init general roc indicator */
  zigzag_init(&zigzag, ZIGZAG, zz_thres, CANDLE_CLOSE);
  timeline_add_indicator(__timeline__(&cluster), __indicator__(&zigzag));
  
  /* Sub-timelines */
  add_timeline_to_cluster(&cluster, "data/AC.yahoo",    "AC", time);
  add_timeline_to_cluster(&cluster, "data/ACA.yahoo",   "ACA", time);
  add_timeline_to_cluster(&cluster, "data/AI.yahoo",    "AI", time);
  add_timeline_to_cluster(&cluster, "data/AIR.yahoo",   "AIR", time);
  add_timeline_to_cluster(&cluster, "data/BN.yahoo",    "BN", time);
  add_timeline_to_cluster(&cluster, "data/BNP.yahoo",   "BNP", time);
  add_timeline_to_cluster(&cluster, "data/CA.yahoo",    "CA", time);
  add_timeline_to_cluster(&cluster, "data/CAP.yahoo",   "CAP", time);
  add_timeline_to_cluster(&cluster, "data/CS.yahoo",    "CS", time);
  add_timeline_to_cluster(&cluster, "data/DG.yahoo",    "DG", time);
  add_timeline_to_cluster(&cluster, "data/EI.yahoo",    "EI", time);
  add_timeline_to_cluster(&cluster, "data/EN.yahoo",    "EN", time);
  add_timeline_to_cluster(&cluster, "data/ENGI.yahoo",  "ENGI", time);
  add_timeline_to_cluster(&cluster, "data/FP.yahoo",    "FP", time);
  add_timeline_to_cluster(&cluster, "data/FR.yahoo",    "FR", time);
  add_timeline_to_cluster(&cluster, "data/GLE.yahoo",   "GLE", time);
  add_timeline_to_cluster(&cluster, "data/KER.yahoo",   "KER", time);
  add_timeline_to_cluster(&cluster, "data/LHN.yahoo",   "LHN", time);
  add_timeline_to_cluster(&cluster, "data/LI.yahoo",    "LI", time);
  add_timeline_to_cluster(&cluster, "data/LR.yahoo",    "LR", time);
  add_timeline_to_cluster(&cluster, "data/MC.yahoo",    "MC", time);
  add_timeline_to_cluster(&cluster, "data/ML.yahoo",    "ML", time);
  add_timeline_to_cluster(&cluster, "data/MT.yahoo",    "MT", time);
  add_timeline_to_cluster(&cluster, "data/NOKIA.yahoo", "NOKIA", time);
  add_timeline_to_cluster(&cluster, "data/OR.yahoo",    "OR", time);
  add_timeline_to_cluster(&cluster, "data/ORA.yahoo",   "ORA", time);
  add_timeline_to_cluster(&cluster, "data/PUB.yahoo",   "PUB", time);
  add_timeline_to_cluster(&cluster, "data/RI.yahoo",    "RI", time);
  add_timeline_to_cluster(&cluster, "data/RNO.yahoo",   "RNO", time);
  add_timeline_to_cluster(&cluster, "data/SAF.yahoo",   "SAF", time);
  add_timeline_to_cluster(&cluster, "data/SAN.yahoo",   "SAN", time);
  add_timeline_to_cluster(&cluster, "data/SGO.yahoo",   "SGO", time);
  add_timeline_to_cluster(&cluster, "data/SOLB.yahoo",  "SOLB", time);
  add_timeline_to_cluster(&cluster, "data/SU.yahoo",    "SU", time);
  add_timeline_to_cluster(&cluster, "data/SW.yahoo",    "SW", time);
  add_timeline_to_cluster(&cluster, "data/TEC.yahoo",   "TEC", time);
  add_timeline_to_cluster(&cluster, "data/UG.yahoo",    "UG", time);
  add_timeline_to_cluster(&cluster, "data/UL.yahoo",    "UL", time);
  add_timeline_to_cluster(&cluster, "data/VIE.yahoo",   "VIE", time);
  add_timeline_to_cluster(&cluster, "data/VIV.yahoo",   "VIV", time);
  
  /* Now create sim */
  sim_init(&sim, &cluster);
  sim_run(&sim, sim_feed);
  sim_display_report(&sim);
  
  return 0;
}
