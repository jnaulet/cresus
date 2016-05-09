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

  struct timeline *t;
  
  s->cluster = c;
  /* Init slists */
  list_head_init(&s->list_position_to_open);
  list_head_init(&s->list_position_opened);
  list_head_init(&s->list_position_to_close);
  list_head_init(&s->list_position_closed);

  /* Other stuff */
  slist_head_init(&s->slist_share);
  /* Copy data */
  __slist_for_each__(&c->slist_timeline, t){
    struct sim_share *share;
    if(sim_share_alloc(share, t))
      __slist_insert__(&s->slist_share, share);
  }

  /* Stat */
  s->factor = 1.0;
  s->nwin = 0;
  s->nloss = 0;
  
  return 0;
}

void sim_release(struct sim *s) {
  /* Nothing to do */
  list_head_release(&s->list_position_to_open);
  list_head_release(&s->list_position_opened);
  list_head_release(&s->list_position_to_close);
  list_head_release(&s->list_position_closed);
}

static void sim_xfer_positions(struct sim *s,
			       list_head_t(struct position) *dst,
			       list_head_t(struct position) *src,
			       position_action_t position_action) {

  struct list *safe;
  struct position *p;
  
  __list_for_each_safe__(src, p, safe){
    __list_del__(p);
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
		      position_t type, double n) {
  
  struct position *p;
  if(position_alloc(p, t, type, n)){
    __list_add_tail__(&s->list_position_to_open, p);
    return 0;
  }

  return -1;
}

int sim_close_position(struct sim *s, struct timeline *t) {
  
  int n = 0;
  struct list *safe;
  struct position *p;

  __list_for_each_safe__(&s->list_position_opened, p, safe){
    if(p->t == t){ /* FIXME : use timeline's name ? */
      __list_del__(p);
      __list_add_tail__(&s->list_position_to_close, p);
      n++;
    }
  }

  return n;
}

int sim_close_all_positions(struct sim *s) {
  
  sim_xfer_positions(s, &s->list_position_to_close,
		     &s->list_position_opened,
		     position_nop);
  
  return 0;
}

int sim_ignore_position(struct sim *s, struct timeline *t) {

  int n = 0;
  struct list *safe;
  struct position *p;
  
  __list_for_each_safe__(&s->list_position_opened, p, safe){
    if(p->t == t){ /* FIXME : use timeline's name ? */
      __list_del__(p);
      n++;
    }
  }
  
  return n;
}

static int sim_find_position(struct sim *s,
			     list_head_t(struct position) *list,
			     struct timeline *t,
			     struct position **ret_p) {
  
  /* TODO : if ret_p != NULL, start with ret_p */
  struct position *p;
  __list_for_each__(list, p) {
    if(p->t == t){
      *ret_p = p;
      return 1;
    }
  }

  *ret_p = NULL;
  return 0;
}

int sim_find_opened_position(struct sim *s, struct timeline *t,
			     struct position **ret_p) {

  return sim_find_position(s, &s->list_position_opened, t, ret_p);
}

static void sim_stat(struct sim *s, struct position *p) {

  double sfactor = 0;
  struct sim_share *share;
  char buf[256], buf2[245];
  double factor = position_factor(p);
  
  /* Find corresponding timeline & adjust */
  __slist_for_each__(&s->slist_share, share){
    if(p->t == share->t){
      share->factor = share->factor * factor;
      if(factor > 1.0) share->nwin++;
      else share->nloss++;
      /* Only one available */
      sfactor = share->factor;
      break;
    }
  }
  
  /* What to do here */
  PR_DBG("%s at [%s:%s] is %.3lf / %.3lf\n",
	 p->t->name, __timeline_entry_str__(p->in, buf, sizeof buf),
	 p->out ? __timeline_entry_str__(p->out, buf2, sizeof buf2) : "N/A",
	 factor, sfactor);
}

static void sim_todo(struct sim *s) {

  struct position *p;
  PR_ERR("Positions to open : \n");
  __list_for_each__(&s->list_position_to_open, p)
    PR_INFO("open %s for %s\n", p->t->name,
	    p->type == POSITION_LONG ? "long" : "short");

  PR_ERR("Positions to close : \n");
  __list_for_each__(&s->list_position_to_close, p)
    PR_INFO("close %s\n", p->t->name);
}

static int sim_report(struct sim *s,
		      list_head_t(struct position) *list) {

  struct position *p;
  struct statistics stat;
  struct sim_share *share;

  int n = 0;
  double total = 0.0;
  
  statistics_init(&stat, 0.0);

  __list_for_each__(list, p){
    /* Feed stats for any list here */
    statistics_feed(&stat, position_value(p));
    sim_stat(s, p);
  }

  statistics_printf(&stat);
  statistics_release(&stat);

  /* Display factors */
  PR_ERR("timeline factor nwin nloss\n");
  __slist_for_each__(&s->slist_share, share){
    PR_INFO("%.8s %.3lf %d %d\n", share->t->name,
	    share->factor, share->nwin, share->nloss);

    s->factor += share->factor;
    s->nwin += share->nwin;
    s->nloss += share->nloss;
    /* Inc */
    n++;
  }

  /* Finish stats */
  s->factor = s->factor / n;
  PR_WARN("total %.3lf %d %d\n", s->factor, s->nwin, s->nloss);
  
  return 0;
}

int sim_display_report(struct sim *s) {
  
  PR_ERR("Statistics for still opened positions\n");
  //sim_report(s, &s->list_position_opened);

  PR_ERR("Statistics for closed positions\n");
  sim_report(s, &s->list_position_closed);

  sim_todo(s);

  /* Some cool info */
  struct timeline_entry *e =
    __list_self__(__timeline__(s->cluster)->list_entry.next);
  struct candle *first = __timeline_entry_self__(e);
  /* Next */
  e =  __list_self__(__timeline__(s->cluster)->list_entry.prev);
  struct candle *last = __timeline_entry_self__(e);
  PR_ERR("factor to match : %.2lf\n", last->close / first->open);
  
  return 0;
}
