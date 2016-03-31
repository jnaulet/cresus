//
//  input.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 20/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#ifndef __Cresus_EVO__input__
#define __Cresus_EVO__input__

#include "candle.h"

/* Typedefs */
struct input;
typedef int (*input_read_ptr)(struct input *in, struct candle *candle);

struct input {
  input_read_ptr read;
};

int input_init(struct input *in, input_read_ptr read);
void input_free(struct input *in);

int input_read(struct input *in, struct candle *candle);

#endif /* defined(__Cresus_EVO__input__) */
