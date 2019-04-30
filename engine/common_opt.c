/*
 * Cresus EVO - common_opt.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 18.04.2019
 * Copyright (c) 2018 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include <getopt.h>
#include <stdlib.h>

#include "engine/common_opt.h"
#include "framework/verbose.h"

int common_opt_init(struct common_opt *ctx, const char *optstring)
{
  memset(ctx, 0, sizeof(*ctx));
  /* Create custom optstring */
  strcpy(ctx->optstring, COMMON_OPTSTRING);
  strcat(ctx->optstring, optstring);
  
  return 0;
}

void common_opt_release(struct common_opt *ctx)
{
}

static time64_t common_opt_time64(struct common_opt *ctx, char *str)
{
  /* Time info format : YYYY-MM-DD (date +%Y-%M-%d) */
  char *Y = strsep(&str, "-");
  char *M = strsep(&str, "-");
  char *d = str; /* End */
  return TIME64_INIT(atoi(Y), atoi(M), atoi(d), 0, 0, 0, 0);
}

int common_opt_getopt(struct common_opt *ctx, int argc, char **argv)
{
  int c;
  
  switch((c = getopt(argc, argv, ctx->optstring))){
  case 'c':
    COMMON_OPT_SET(&ctx->csv_output, i, 1);
    break;

  case 'f':
    sscanf(optarg, "%lf", &ctx->transaction_fee.d);
    ctx->transaction_fee.set = 1;
    break;
    
  case 'o':
    COMMON_OPT_SET(&ctx->input_type, s, optarg);
    break;

  case 'q':
    COMMON_OPT_SET(&ctx->quiet, i, 1);
    break;
    
  case 'v':
    VERBOSE_LEVEL(DBG);
    break;
    
  case 'F':
    COMMON_OPT_SET(&ctx->fixed_amount, i, atoi(optarg));
    break;
    
  case 'S':
    COMMON_OPT_SET(&ctx->start_time, t,
                   common_opt_time64(ctx, optarg));
    break;
    
  case 'E':
    COMMON_OPT_SET(&ctx->end_time, t,
                   common_opt_time64(ctx, optarg));
    break;
    
  default:
    /* Ignore unknown opt */
    break;
  }
  
  return c;
}
