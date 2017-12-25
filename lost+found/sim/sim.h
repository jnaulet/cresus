/*
 * Cresus EVO - sim.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/05/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef SIM_H
#define SIM_H

#include "sim/position.h"
#include "engine/cluster.h"
#include "framework/list.h"
#include "framework/slist.h"

#define sim_share_alloc(s, t)				\
  DEFINE_ALLOC(struct sim_share, s, sim_share_init, t)
#define sim_share_free(s)			\
  DEFINE_FREE(s, sim_share_release)

struct sim_share {
  /* This is a simple list */
  __inherits_from_slist__;
  /* With a few elements */
  struct timeline *t;
  double factor;
  int nwin, nloss;
};

static inline int sim_share_init(struct sim_share *s,
				 struct timeline *t) {
  /* super() */
  __slist_super__(s);
  /* params */
  s->t = t;
  /* own data */
  s->factor = 1.0;
  s->nwin = 0;
  s->nloss = 0;
  return 0;
}

static inline void sim_share_release(struct sim_share *s) {
  __slist_release__(s);
}

struct sim {
  struct cluster *cluster;
  list_head_t(struct position) list_position_to_open;
  list_head_t(struct position) list_position_opened;
  list_head_t(struct position) list_position_to_close;
  list_head_t(struct position) list_position_closed;
  /* more "personal" */
  slist_head_t(struct sim_share) slist_share;
  /* Stats */
  double factor;
  int nwin, nloss;
};

/* types */
typedef int (*sim_feed_ptr)(struct sim*, struct cluster*);

/* API */
int sim_init(struct sim *s, struct cluster *c);
void sim_release(struct sim *s);

int sim_run(struct sim *s, sim_feed_ptr feed);
int sim_open_position(struct sim *s, struct timeline *t,
		      position_t type, double n);
int sim_close_position(struct sim *s, struct timeline *t);
int sim_close_all_positions(struct sim *s);
int sim_ignore_position(struct sim *s, struct timeline *t);

int sim_find_opened_position(struct sim *s, struct timeline *t,
			     struct position **ret_p) ;

int sim_display_report(struct sim *s);

#endif
