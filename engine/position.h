/*
 * Cresus EVO - position.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04.05.2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef POSITION_H
#define POSITION_H

#include "engine/cert.h"
#include "framework/list.h"
#include "framework/alloc.h"

typedef enum {
  POSITION_BUY,
  POSITION_SELL,
  POSITION_SELLSHORT,
  POSITION_EXITSHORT
} position_t;

/* Shortcuts */
#define BUY       POSITION_BUY
#define SELL      POSITION_SELL
#define SELLSHORT POSITION_SELLSHORT
#define EXITSHORT POSITION_EXITSHORT

typedef enum {
  POSITION_REQ_SHARES,
  POSITION_REQ_CASH
} position_req_t;

/* Shortcuts */
#define SHARES POSITION_REQ_SHARES
#define CASH   POSITION_REQ_CASH

typedef enum {
  POSITION_REQUESTED,
  POSITION_CONFIRMED,
  POSITION_STOPPED,
  POSITION_DESTROY
} position_status_t;

#define position_alloc(p,  type, content, n, cert)			\
  DEFINE_ALLOC(struct position, p, position_init,			\
	       type, content, n, (cert))
#define position_free(p)			\
  DEFINE_FREE(p, position_release)

struct position {
  /* slistable */
  __inherits_from_list__;
  /* Internal data */
  position_t type;
  position_status_t status;
  /* Request part */
  position_req_t req;
  union {
    double cash;
    int shares;
  } request;

  double n; /* Assets number */
  double value; /* Chart value */
  struct cert cert; /* Certificates options */
};

int position_init(struct position *ctx, position_t type,
		  position_req_t req, double n, struct cert *cert);
void position_release(struct position *ctx);

/* Accessors */
#define position_set_n(ctx, param)		\
  (ctx)->n = param
#define position_set_value(ctx, param)		\
  (ctx)->value = param
#define position_confirm(ctx)			\
  (ctx)->status = POSITION_CONFIRMED
#define position_destroy(ctx)			\
  (ctx)->status = POSITION_DESTROY

double position_unit_value(struct position *ctx); /* FIXME : better name */
double position_current_value(struct position *ctx, double cur);

#endif
