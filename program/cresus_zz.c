#include <stdio.h>
#include <float.h>
#include <string.h>

#include "atr.h"
#include "google.h"
#include "candle.h"
#include "zigzag.h"
#include "portfolio.h"
#include "statistics.h"

/* ZZ */
#define ZIGZAG_THRES  0.1
#define ZIGZAG_TYPE   ZIGZAG_PERCENT
/* STATS */
#define OP_COST 5.0 /* Euros */

/* Moving ref object */

#define MOVING_REF_MAX 3
#define MOVING_REF_BACK(mr, n)				\
  (mr->index + MOVING_REF_MAX - n) % MOVING_REF_MAX

typedef enum {
  MOVING_REF_NONE,
  MOVING_REF_UP,
  MOVING_REF_DOWN,
} moving_ref_t;

struct moving_ref {
  int index, count;
  struct candle ref[MOVING_REF_MAX];
};

int moving_ref_init(struct moving_ref *m)
{
  m->count = 0;
  m->index = 0;

  return 0;
}

void moving_ref_feed(struct moving_ref *m, struct candle *cdl)
{
  memcpy(&m->ref[m->index], cdl, sizeof *cdl);
  m->index = (m->index + 1) % MOVING_REF_MAX;
  m->count++;
}

moving_ref_t moving_ref_pattern_find(struct moving_ref *m)
{
  if(m->count < MOVING_REF_MAX)
    return MOVING_REF_NONE;

  /* Find patterns */
  struct candle *ref0 = &m->ref[MOVING_REF_BACK(m, 2)];
  struct candle *ref1 = &m->ref[MOVING_REF_BACK(m, 1)];
  struct candle *ref2 = &m->ref[MOVING_REF_BACK(m, 0)];
  
  if(ref0->close < ref1->close &&
     ref2->close > ref0->close &&
     ref2->close < ref1->close)
    /* The up ^ */
    return MOVING_REF_UP;

  if(ref0->close > ref1->close &&
     ref2->close < ref0->close &&
     ref2->close > ref1->close)
    /* The down v */
    return MOVING_REF_DOWN;

  return MOVING_REF_NONE;
}

/* Main loop */

int main(int argc, char **argv)
{
  struct google g;
  struct candle cdl;
  struct moving_ref mr;

  double last_ref;
  zigzag_dir_t last_dir = ZIGZAG_DIR_NONE;

  /* indicators */
  struct zigzag zz;
  struct portfolio pf;
  struct statistics stats;
  
  if(google_init(&g, "./google.csv") != -1 &&
     google_read(&g, &cdl, 1)){

    /* Init */
    zigzag_init(&zz, ZIGZAG_TYPE, ZIGZAG_THRES, &cdl);

    portfolio_init(&pf);
    moving_ref_init(&mr);
    statistics_init(&stats, OP_COST);
    
  }else /* Can't open file */
    return -1;

  for(;;){

    /* We get next day data */
    if(!google_read(&g, &cdl))
      break;

    /* After we got all data */
    if(zigzag_feed(&zz, &cdl) != last_dir){
      
      double value;
      struct candle ref;
      moving_ref_t pattern;

      /* Set parameters */
      last_dir = zigzag_compute(&zz, &ref);
      moving_ref_feed(&mr, &ref);

      switch((pattern = moving_ref_pattern_find(&mr))){
      case MOVING_REF_UP :
	portfolio_buy(&pf, PORTFOLIO_LONG, cdl.close);
	printf("L %s for %.2lf\n", candle_localtime_str(&cdl), cdl.close);
	break;

      case MOVING_REF_DOWN :
	value = portfolio_sell/*_all*/(&pf, cdl.close);
	statistics_accumulate(&stats, value);
	break;
      }
      
      /* Debug */
      if(pattern != MOVING_REF_NONE)
	printf("Pattern %s found at %s\n",
	       pattern == MOVING_REF_UP ? "up" : "down",
	       candle_localtime_str(&cdl));
    }
  }

  /* Last sell */
  statistics_printf(&stats);

  /* Free all */
  google_free(&g);
  zigzag_free(&zz);
  portfolio_free(&pf);
  statistics_free(&stats);

  return 0;
}
