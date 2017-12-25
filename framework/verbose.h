/*
 * Cresus EVO - verbose.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 26/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef VERBOSE_H
#define VERBOSE_H

extern int __verbose__;
extern int __verbose_color__;

/* Values */
#define DBG  0
#define INFO 1
#define WARN 2
#define ERR  3
#define NONE 255

/* Fonts */
#define DBG_FONT  "\x1b[1;36m"
#define INFO_FONT "\x1b[1;32m"
#define WARN_FONT "\x1b[1;33m"
#define ERR_FONT  "\x1b[1;31m"
#define NO_FONT   "\x1b[1;0m"

#define PR(level, format, font, ...)					\
  if(level >= __verbose__)						\
    if(__verbose_color__)						\
      fprintf(stderr, font "%s: " format NO_FONT,			\
	      __func__, ##__VA_ARGS__);					\
      else								\
	fprintf(stderr, "%s: " format,					\
		__func__, ##__VA_ARGS__)

#define VERBOSE_LEVEL(level) __verbose__ = level
#define VERBOSE_COLOR(bool) __verbose_color__ = bool

#define PR_DBG(format, ...)  PR(DBG, format, DBG_FONT, ##__VA_ARGS__)
#define PR_INFO(format, ...) PR(INFO, format, INFO_FONT, ##__VA_ARGS__)
#define PR_WARN(format, ...) PR(WARN, format, WARN_FONT, ##__VA_ARGS__)
#define PR_ERR(format, ...)  PR(ERR, format, ERR_FONT, ##__VA_ARGS__)

#define PR_BUY(level, value, format, ...)		\
  fprintf(stdout, "Buy %.2lf %d " format "\n", value,	\
	  level, ##__VA_ARGS__)

#endif
