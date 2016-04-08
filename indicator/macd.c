/*
 * Cresus EVO - macd.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>

#include "macd.h"
#include "engine/candle.h"

static int macd_feed(struct indicator *i, struct timeline_entry *e) {
  
  struct macd *m = __indicator_self__(i);
  struct candle *c = __timeline_entry_self__(e);

  /* Set 2 main averages */
  double fast = average_update(&m->fast, c->close);
  double slow = average_update(&m->slow, c->close);
  if(average_is_available(&m->fast) &&
     average_is_available(&m->slow)){
    /* Compute signal */
    double value = (fast - slow);
    double signal = average_update(&m->signal, value);
    if(average_is_available(&m->signal)){
      struct macd_entry *entry;
      if(macd_entry_alloc(entry, i, value, signal))
	candle_add_indicator_entry(c, __indicator_entry__(entry));
    }
  }
  
  /* State machine & events here */
  
  return 0;
}

int macd_init(struct macd *m, indicator_id_t id,
	      int fast_p, int slow_p, int signal_p) {

  /* Super */
  __indicator_super__(m, id, macd_feed);
  __indicator_set_string__(m, "macd[%d,%d,%d]", fast_p, slow_p, signal_p);

  /* Seed */
  average_init(&m->fast, AVERAGE_EXP, fast_p);
  average_init(&m->slow, AVERAGE_EXP, slow_p);
  average_init(&m->signal, AVERAGE_EXP, signal_p);
  
  return 0;
}

void macd_release(struct macd *m)
{
  __indicator_release__(m);
  average_release(&m->fast);
  average_release(&m->slow);
  average_release(&m->signal);
}
