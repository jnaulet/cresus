/*
 * Cresus EVO - position.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04.05.2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include "position.h"
#include "framework/verbose.h"

int position_init(struct position *ctx, position_t type,
		  position_req_t req, double n, struct cert *cert)
{
  /* super() */
  __list_super__(ctx);

  ctx->req = req;
  ctx->type = type;
  ctx->status = POSITION_REQUESTED;
  /* Content */
  if(req == POSITION_REQ_SHARES) ctx->request.shares = n;
  else ctx->request.cash = n;
  /* Value */
  ctx->value = 0;
  /* Certificates */
  if(!cert) cert_zero(&ctx->cert);
  else memcpy(&ctx->cert, cert, sizeof(ctx->cert));
  
  return 0;
}

void position_release(struct position *ctx)
{
  __list_release__(ctx);
}

double position_unit_value(struct position *ctx)
{
  if(ctx->type == POSITION_BUY ||
     ctx->type == POSITION_SELL)
    return cert_long_value(&ctx->cert, ctx->value);
  
  if(ctx->type == POSITION_SELLSHORT ||
     ctx->type == POSITION_EXITSHORT)
    return cert_short_value(&ctx->cert, ctx->value);
  
  return 0.0;
}

double position_current_value(struct position *ctx, double cur)
{
  if(ctx->status != POSITION_CONFIRMED)
    goto out;
  
  if(ctx->type == POSITION_BUY ||
     ctx->type == POSITION_SELL)
    return ctx->n * ((cur - ctx->cert.funding) / ctx->cert.ratio);
  
  if(ctx->type == POSITION_SELLSHORT ||
     ctx->type == POSITION_EXITSHORT)
    return ctx->n * ((ctx->cert.funding - cur) / ctx->cert.ratio);

 out:
  return 0.0;
}
