/*
 * Cresus EVO - cresus_xx.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <float.h>
#include <string.h>

#include "swing.h"
#include "google.h"
#include "candle.h"

#define FILE "/dev/stdin"
#define CANDLE_BASE 10 /* Minutes */

/* Patterns */
#define PATTERN_MAX 3
#define __PATTERN_BACK(n)					\
  &p->pool[((p->index - n) + PATTERN_MAX) % PATTERN_MAX] 

struct pattern  {
  int count;
  int index;
  struct candle pool[PATTERN_MAX];
};

static double __pattern_detect(struct pattern *p)
{
  if(p->count > PATTERN_MAX){
    
    struct candle *s0 = __PATTERN_BACK(2);
    struct candle *s1 = __PATTERN_BACK(1);
    struct candle *s2 = __PATTERN_BACK(0);

    return s2->close - s0->close;
  }

  return 0.0;
}

int pattern_init(struct pattern *p, struct candle *cdl)
{
  p->index = 0;
  p->count = 1;
  memcpy(&p->pool[p->index], cdl, sizeof *cdl);
  
  return 0;
}

void pattern_release(struct pattern *p)
{
}

int pattern_feed(struct pattern *p, struct candle *cdl)
{
  p->count++;
  p->index = (p->index + 1) % PATTERN_MAX;
  memcpy(&p->pool[p->index], cdl, sizeof *cdl);
  
  return __pattern_detect(p);
}

int main(int argc, char **argv)
{
  struct google g;
  struct candle cdl;
  struct swing swing;
  struct pattern pattern;
  
  /* Misc */
  if(google_init(&g, FILE) != -1 &&
     google_read(&g, &cdl, CANDLE_BASE)){
    
    /* Init */
    swing_init(&swing, &cdl);
    pattern_init(&pattern, &cdl); /* FIXME ? */
    
  }else /* Can't open file */
    return -1;

  for(;;){

    double value;
    struct candle *res;

    /* We get next day data */
    if(!google_read(&g, &cdl, CANDLE_BASE))
      break;
    
    printf("%s - %.2lf %.2lf %.2lf %.2lf\n", candle_localtime_str(&cdl),
	   cdl.open, cdl.high, cdl.low, cdl.close);

    if((res = swing_feed(&swing, &cdl))){
      printf("%s - %s swing type found at %.2lf (%.2lf)\n",
	     candle_localtime_str(res),
	     (swing.type == SWING_TOP ? "top" : "bottom"),
	     res->close, cdl.close);

      /* Feed pattern */
      if((value = pattern_feed(&pattern, res)) != 0.0)
	printf("%s - %.2lf pattern detected\n", candle_localtime_str(res),
	       value);
    }
  }
  
  /* Free all */
  google_release(&g);
  return 0;
}
