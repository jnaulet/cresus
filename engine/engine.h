/*
 * Cresus EVO - engine.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/21/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__engine__
#define __Cresus_EVO__engine__

#include "framework/input.h"
#include "framework/module.h"

#define ENGINE_MAX_MODULE 4

struct engine {
  
  struct input *input;
  
  /* Indicators array */
  struct module *module[ENGINE_MAX_MODULE];
  size_t modules;
};

int engine_init(struct engine *e, struct input *in);
void engine_free(struct engine *e);

int engine_add_module(struct engine *e, struct module *m);
void engine_run(struct engine *e, int n);

#endif /* defined(__Cresus_EVO__engine__) */
