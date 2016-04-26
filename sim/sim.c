/*
 * Cresus EVO - sim.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/05/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "sim/sim.h"
#include "sim/statistics.h"
#include "framework/verbose.h"

int sim_init(struct sim *s, struct cluster *c) {

  s->cluster = c;
  /* Init slists */
  list_init(&s->list_position_to_open);
  list_init(&s->list_position_opened);
  list_init(&s->list_position_to_close);
  list_init(&s->list_position_closed);

  return 0;
}

void sim_free(struct sim *s) {
  /* Nothing to do */
}

static void sim_xfer_positions(struct sim *s,
			       list_head_t(struct position) *dst,
			       list_head_t(struct position) *src,
			       position_action_t position_action) {
  
  struct position *p;
  __list_for_each__(src, p){
    __list_del__(p); /* FIXME : use safe ? */
    /* Insert in dst */
    __list_add_tail__(dst, p);
    /* Act on position */
    position_action(p);
  }
}

int sim_run(struct sim *s, sim_feed_ptr feed) {

  while(cluster_step(s->cluster) != -1){
    /* First : check if there are opening positions */
    sim_xfer_positions(s, &s->list_position_opened,
		       &s->list_position_to_open,
		       position_in);
    /* Second : check if there are closing positions */
    sim_xfer_positions(s, &s->list_position_closed,
		       &s->list_position_to_close,
		       position_out);
    /* Third  : feed the sim */
    feed(s, s->cluster);
  }

  return 0;
}

int sim_open_position(struct sim *s, struct timeline *t, 
		      position_t type, int n) {
  
  struct position *p;
  if(position_alloc(p, t, type, n)){
    __list_add__(&s->list_position_to_open, p); /* open could be removed */
    return 0;
  }

  return -1;
}

int sim_close_position(struct sim *s, struct timeline *t) {
  
  int n = 0;
  struct position *p;

  __list_for_each__(&s->list_position_opened, p){
    if(p->t == t){ /* FIXME : use timeline's name ? */
      __list_del__(p); /* FIXME : use safe */
      __list_add_tail__(&s->list_position_to_close, p);
      n++;
    }
  }

  return n;
}

static int sim_report(struct sim *s,
		      list_head_t(struct position) *list) {

  struct position *p;
  struct statistics stat;

  statistics_init(&stat, 0.0);

  __list_for_each__(list, p)
    /* Feed stats for any list here */
    statistics_feed(&stat, position_value(p));

  statistics_printf(&stat);
  statistics_release(&stat);

  return 0;
}

int sim_display_report(struct sim *s) {
  
  PR_INFO("Statistics for still opened positions\n");
  sim_report(s, &s->list_position_opened);

  PR_INFO("Statistics for closed positions\n");
  sim_report(s, &s->list_position_closed);

  return 0;
}
