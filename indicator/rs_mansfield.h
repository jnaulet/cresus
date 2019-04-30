/*
 * Cresus EVO - rs_mansfield.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef RS_MANSFIELD_H
#define RS_MANSFIELD_H

/*
 * Relative Strength (Mansfield)
 * Fomula :
 * RSM = ((RSD(today) / mma(RSD(today), n)) - 1) * 100.0
 */

#include <stdlib.h>

#include "math/average.h"
#include "framework/list.h"
#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

/* Entries */

struct rs_mansfield_n3 {
  /* As below */
  __inherits_from__(struct indicator_n3);
  /* Single value */
  double value;
  /* More info */
  double direction;
  /* Events ? */
};

#define rs_mansfield_n3_alloc(ctx, parent, value, direction)         \
  DEFINE_ALLOC(struct rs_mansfield_n3, ctx,                          \
	       rs_mansfield_n3_init, parent, value,			\
	       direction)
#define rs_mansfield_n3_free(ctx)            \
  DEFINE_FREE(ctx, rs_mansfield_n3_release)

static inline int rs_mansfield_n3_init(struct rs_mansfield_n3 *ctx,
					  struct indicator *parent,
					  double value, double direction)
{
  __indicator_n3_init__(ctx, parent);
  ctx->value = value;
  ctx->direction = direction;
  return 0;
}

static inline void
rs_mansfield_n3_release(struct rs_mansfield_n3 *ctx)
{
  __indicator_n3_release__(ctx);
}

/* Object */

#define rs_mansfield_alloc(ctx, uid, period, ref_track_uid)             \
  DEFINE_ALLOC(struct rs_mansfield, ctx, rs_mansfield_init, uid,        \
               period, ref_track_uid)
#define rs_mansfield_free(ctx)			\
  DEFINE_FREE(ctx, rs_mansfield_release)

struct rs_mansfield {
  /* As always, inherits from indicator */
  __inherits_from__(struct indicator);
  
  double value;
  struct average mma;
  unique_id_t ref_track_uid;
};

int rs_mansfield_init(struct rs_mansfield *ctx, unique_id_t uid, int period, unique_id_t ref_track_uid);
void rs_mansfield_release(struct rs_mansfield *ctx);

/* Indicator-specific */
double rs_mansfield_value(struct rs_mansfield *ctx);

#endif
