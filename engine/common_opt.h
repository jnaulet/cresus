/*
 * Cresus EVO - common_opt.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 18/04/2019
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef COMMON_OPT_H
#define COMMON_OPT_H

#include "framework/time64.h"

struct common_opt_tuple {
  int set;
  union {
    int i;
    char *s;
    double d;
    time64_t t;
  };
};

#define COMMON_OPT_SET(tuple, type, val)         \
  { (tuple)->set = 1;                            \
    (tuple)->type = (val); };

#define COMMON_OPTSTRING "cf:o:vF:S:E:"
/*
 * -c enable csv output (for gnuplot)
 * -f <transaction fee>
 * -o <file type>
 * -q quiet
 * -v verbose
 * -F <fixed amount>
 * -S <start time> (format : YYYY-MM-DD)
 * -E <end time> (format : YYYY-MM-DD)
 */

struct common_opt {
  /* Internals */
  char optstring[256];
  /* Data */
  struct common_opt_tuple input_type;
  struct common_opt_tuple fixed_amount;
  struct common_opt_tuple start_time;
  struct common_opt_tuple end_time;
  struct common_opt_tuple transaction_fee;
  struct common_opt_tuple csv_output;
  struct common_opt_tuple quiet;
};

int common_opt_init(struct common_opt *ctx, const char *optstring);
void common_opt_release(struct common_opt *ctx);

int common_opt_getopt(struct common_opt *ctx, int argc, char **argv);

#endif
