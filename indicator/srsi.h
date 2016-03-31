//
//  srsi.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 20/11/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#ifndef __Cresus_EVO__srsi__
#define __Cresus_EVO__srsi__

#include "framework/indicator.h"

struct srsi {
  struct indicator parent;
  
  int max;
  int len;
  
  struct {
    int bull;
    int bear;
    int total;
  } *array;
};

int srsi_init(struct srsi *s, int max, const struct candle *seed);
void srsi_free(struct srsi *s);

int srsi_feed(struct indicator *i, const struct candle *candle);

#endif /* defined(__Cresus_EVO__srsi__) */
