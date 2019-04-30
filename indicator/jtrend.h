/*
 * Cresus EVO - jtrend.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 02/05/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef JTREND_H
#define JTREND_H

#include "math/average.h"
#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"
/* Sub-indicators */
#include "indicator/roc.h"

struct jtrend_n3 {
  /* Always */
  __inherits_from__(struct indicator_n3);
  /* Value type ? */
  double value;
  double ref_value;
};

#define jtrend_n3_alloc(n3, parent, value, ref_value)		\
  DEFINE_ALLOC(struct jtrend_n3, n3, jtrend_n3_init,		\
	       parent, value, ref_value)
#define jtrend_n3_free(n3)		\
  DEFINE_FREE(n3, jtrend_n3_release)

static inline int jtrend_n3_init(struct jtrend_n3 *n3,
				    struct indicator *parent,
				    double value, double ref_value)
{
  __indicator_n3_init__(n3, parent);
  n3->value = value;
  n3->ref_value = ref_value;
  return 0;
}

static inline void jtrend_n3_release(struct jtrend_n3 *n3)
{
  __indicator_n3_release__(n3);
}

/* Main object */

#define jtrend_alloc(jt, uid, period, average)                           \
  DEFINE_ALLOC(struct jtrend, jt, jtrend_init, uid, period, average)
#define jtrend_free(jt)				\
  DEFINE_FREE(jt, jtrend_release)

struct jtrend {
  /* Anyway */
  __inherits_from__(struct indicator);
  /* Some other data */
  struct roc roc;
  struct roc roc_ref;
  /* Referecne flow */
  unique_id_t ref_track_uid;
};

int jtrend_init(struct jtrend *ctx, unique_id_t uid, int period, int average, unique_id_t ref_track_uid);
void jtrend_release(struct jtrend *ctx);

#endif
