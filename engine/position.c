/*
 * Cresus EVO - position.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04.05.2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "position.h"
#include "framework/verbose.h"

int position_init(struct position *p, struct timeline *t,
		  position_t type, double n) {

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

  int ret;
  struct timeline_entry *entry;
  if((ret = timeline_entry_current(p->t, &entry)) < 0){
    PR_WARN("%s position for %s can't be executed\n",
	    p->type == POSITION_LONG ? "LONG" : "SHORT", p->t->name);
    
    goto out;
  }

  /* Set return value */
  *c = __timeline_entry_self__(entry);
  
 out:
  return ret;
}

int position_in(struct position *p) {

  return position_exec(p, &p->in);
}

int position_out(struct position *p) {

  return position_exec(p, &p->out);
}

int position_nop(struct position *p) {

  return 0;
}

static double position_out_value(struct position *p) {

  double out;
  
  if(p->out == NULL){
    /* If we're not out of position */
    struct candle *c;
    struct timeline_entry *e;
    
    if(timeline_entry_current(p->t, &e) != -1){
      c = __timeline_entry_self__(e);
      out = c->open;
    }else
      out = p->in->open;
  }else
    out = p->out->open;

  return out;
}

double position_value(struct position *p) {

  double ret;
  double out = position_out_value(p);
  
  /* FIXME : forced candle open value comparison */
  if(p->type == POSITION_LONG)
    ret = (out - p->in->open);
  else
    ret = (p->in->open - out);
  
  return ret;
}

double position_factor(struct position *p) {

  double ret;
  double out = position_out_value(p);
  
  /* FIXME : forced candle open value comparison */
  if(p->type == POSITION_LONG)
    ret = (out / p->in->open);
  else
    ret = (p->in->open / out);
  
  return ret;
}
