#ifndef STACK_H
#define STACK_H

#include <sys/types.h>

struct stack {
  int count, index;

  size_t len;
  void **pool;
};

int stack_init(struct stack *s, size_t len);
void stack_free(struct stack *s);

int stack_push(struct stack *s, void *ptr);
void *stack_pop(struct stack *s, int index);

#endif
