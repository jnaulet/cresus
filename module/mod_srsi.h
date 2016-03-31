//
//  mod_srsi.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 20/11/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#ifndef __Cresus_EVO__mod_srsi__
#define __Cresus_EVO__mod_srsi__

#include "module.h"
#include "candle.h"
#include "srsi.h"

struct mod_srsi {
  struct module parent;
  struct srsi srsi;
};

int mod_srsi_init(struct mod_srsi *m, const struct candle *seed);
void mod_srsi_free(struct mod_srsi *m);

#endif /* defined(__Cresus_EVO__mod_srsi__) */
