//
//  input.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 20/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include "input.h"

int input_init(struct input *in, input_read_ptr read) {
  
  in->read = read;
  return 0;
}

void input_free(struct input *in) {
  
}

int input_read(struct input *in, struct candle *candle) {
  
  return in->read(in, candle);
}
