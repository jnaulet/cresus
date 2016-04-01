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

static int macd_feed(struct indicator *i, struct candle *c) {
  
  struct macd *m = __indicator_self__(i);
  double fast = average_update(&m->fast, c->close);
  double slow = average_update(&m->slow, c->close);
  
  m->result.value = fast - slow;
  m->result.signal = average_update(&m->signal, m->result.value);
  m->result.histogram = m->result.value - m->result.signal;
  
  /* State machine & events here */
  
  return 0;
}

int macd_init(struct macd *m, int fast_p, int slow_p, int signal_p) {

  /* Super */
  __indicator_super__(m, macd_feed);
  __indicator_set_string__(m, "macd[%d, %d, %d]", fast_p, slow_p, signal_p);

  /* Seed */
  average_init(&m->fast, AVERAGE_EXP, fast_p);
  average_init(&m->slow, AVERAGE_EXP, slow_p);
  average_init(&m->signal, AVERAGE_EXP, signal_p);
  
  memset(&m->result, 0, sizeof m->result);
  return 0;
}

void macd_free(struct macd *m)
{
  __indicator_free__(m);
  average_free(&m->fast);
  average_free(&m->slow);
  average_free(&m->signal);
}

const char *macd_str(struct macd *m, char *buf, size_t len)
{
  sprintf(buf, "%.2lf %.2lf (%+.2lf)", m->result.value, m->result.signal,
	  m->result.histogram);
  
  return buf;
}
