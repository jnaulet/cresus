/*
 * Cresus EVO - verbose.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 26/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef VERBOSE_H
#define VERBOSE_H

#include <stdio.h>
#include <systemd/sd-journal.h>

#define PR(level, ...) \
  (void) sd_journal_print(level, __VA_ARGS__)

#define PR_DBG(format, ...)  PR(LOG_DEBUG, format, ##__VA_ARGS__)
#define PR_INFO(format, ...) PR(LOG_INFO, format, ##__VA_ARGS__)
#define PR_WARN(format, ...) PR(LOG_WARNING, format, ##__VA_ARGS__)
#define PR_ERR(format, ...)  PR(LOG_ERR, format, ##__VA_ARGS__)
#define PR_STAT(format, ...) fprintf(stdout, format, ##__VA_ARGS__)

#endif
