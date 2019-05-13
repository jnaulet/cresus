#ifndef INPUT_WRAPPER_H
#define INPUT_WRAPPER_H

#include <string.h>

#include "input/b4b.h"
#include "input/mdgms.h"
#include "input/xtrade.h"
#include "input/kraken.h"
#include "input/yahoo_v7.h"
#include "input/euronext.h"

/* Beware : dangerous */
union input_wrapper {
  struct input *input;
  struct b4b *b4b;
  struct mdgms *mdgms;
  struct xtrade *xtrade;
  struct euronext *euronext;
  struct kraken *kraken;
  struct yahoo_v7 *yahoo_v7;
};

static inline struct input *
input_wrapper_create(const char *filename, const char *filetype)
{
  union input_wrapper ret;
  
  if(!strcmp("b4b", filetype)) b4b_alloc(ret.b4b, filename);
  else if(!strcmp("mdgms", filetype)) mdgms_alloc(ret.mdgms, filename);
  else if(!strcmp("xtrade", filetype)) xtrade_alloc(ret.xtrade, filename);
  else if(!strcmp("euronext", filetype)) euronext_alloc(ret.euronext, filename);
  else if(!strcmp("kraken", filetype)) kraken_alloc(ret.kraken, filename);
  /* Default yahoo v7 */
  else yahoo_v7_alloc(ret.yahoo_v7, filename);
  return ret.input;
}

static inline struct input *
input_wrapper_create_by_ext(const char *filename)
{
  /* TODO : check */
  char *ext = strrchr(filename, '.') + 1;
  return input_wrapper_create(filename, ext);
}

#endif
