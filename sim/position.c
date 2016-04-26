/*
 * Cresus EVO - position.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04.05.2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "position.h"

int position_init(struct position *p, struct timeline *t,
		  position_t type, int n) {

  /* super() */
  __list_super__(p);
  
  p->t = t;
  p->in = NULL;
  p->out = NULL;
  
  p->n = n;
  p->type = type;
  
  return 0;
}

void position_release(struct position *p) {

  __list_release__(p);
  p->in = NULL;
  p->out = NULL;
}

/* FIXME : find better names */

static int position_exec(struct position *p, struct candle **c) {

  struct timeline_entry *entry;
  int ret = timeline_entry_current(p->t, &entry);
  *c = __timeline_entry_self__(entry);
  
  return ret;
}

int position_in(struct position *p) {

  return position_exec(p, &p->in);
}

int position_out(struct position *p) {

  return position_exec(p, &p->out);
}

double position_value(struct position *p) {

  double out, ret;
  
  if(p->out == NULL){
    /* If we're not out of position */
    struct candle *c;
    struct timeline_entry *e;
    /* TODO : check */
    timeline_entry_current(p->t, &e);
    c = __timeline_entry_self__(e);
    out = c->open;
  }else
    out = p->out->open;

  /* FIXME : forced candle open value comparison */
  if(p->type == POSITION_LONG)
    ret = (out - p->in->open);
  else
    ret = (p->in->open - out);
  
  return ret;
}
