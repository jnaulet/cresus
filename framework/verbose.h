/*
 * Cresus EVO - verbose.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 26/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef VERBOSE_H
#define VERBOSE_H

#define PR(format, esc, ...)						\
  fprintf(stderr, esc "%s: " format "\x1b[1;0m", __func__, ##__VA_ARGS__)

#define PR_DBG(format, ...)  PR(format, "\x1b[1;36m", ##__VA_ARGS__)
#define PR_INFO(format, ...) PR(format, "\x1b[1;32m",  ##__VA_ARGS__)
#define PR_WARN(format, ...) PR(format, "\x1b[1;33m", ##__VA_ARGS__)
#define PR_ERR(format, ...)  PR(format, "\x1b[1;31m", ##__VA_ARGS__)

#endif
