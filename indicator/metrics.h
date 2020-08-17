/*
 * Cresus v2 - metrics.h
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/17/2020
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "framework/alloc.h"
#include "framework/verbose.h"
#include "framework/indicator.h"

struct metrics_n3 {
  struct indicator_n3 indicator_n3; /* Inherits from indicator_n3 */
  /* General */
  double shares; /* Outstanding shares */
  double market_cap; /* Market capitalization */
  double working_cap; /* Working capital */
  double book_value; /* Book value */
  /* Metrics / ratios */
  double pe; /* P/E metrics */
  double pbv; /* Price to book value */
  double eps; /* Earnings per share */
  double roe; /* Return on equity */
  double roce; /* Return on capital employed */
  double bvps; /* Book value per share */
  double cr; /* Current ratio */
  double qr; /* Quick ratio */
  double de; /* Debt / equity ratio */
};

#define metrics_n3_alloc(ctx, parent)                           \
  DEFINE_ALLOC(struct metrics_n3, ctx, metrics_n3_init, parent)
#define metrics_n3_free(ctx)                    \
  DEFINE_FREE(ctx, metrics_n3_release)

static inline int metrics_n3_init(struct metrics_n3 *ctx,
                                  struct indicator *parent)
{
  indicator_n3_init(&ctx->indicator_n3, parent); /* Super */
  return 0;
}

static inline void metrics_n3_release(struct metrics_n3 *ctx)
{
  indicator_n3_release(&ctx->indicator_n3);
}

static inline void metrics_n3_display(struct metrics_n3 *ctx)
{
  PR_INFO("shares %.2lf cap %.2lf working_cap %.2lf book_value %.2lf\n",
          ctx->shares, ctx->market_cap, ctx->working_cap, ctx->book_value);
  PR_INFO("PE %.2lf PBV %.2lf EPS %.2lf ROE %.2lf ROCE %.2lf BVPS %.2lf CR %.2lf QR %.2lf DE %.2lf\n",
	  ctx->pe, ctx->pbv, ctx->eps, ctx->roe, ctx->roce, ctx->bvps, ctx->cr, ctx->qr, ctx->de);
}

struct metrics {
  struct indicator indicator; /* Inherits from indicator */
};

#define metrics_alloc(ctx, uid)                         \
  DEFINE_ALLOC(struct metrics, ctx, metrics_init, uid)
#define metrics_free(ctx)                       \
  DEFINE_FREE(ctx, metrcis_release)

int metrics_init(struct metrics *ctx, unique_id_t uid);
void metrics_release(struct metrics *ctx);
