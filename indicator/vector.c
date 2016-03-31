#include <stdio.h>
#include "stack.h"

int stack_init(struct stack *s, size_t len)
{
  v->count = 0;
  v->index = 0;

  v->len = len;
  if(!(v->pool = malloc(v->len * sizeof(void*))))
    return -1;

  return 0;
}

void stack_free(struct stack *s)
{
  if(v->pool) free(v->pool);
}

int stack_push(struct stack *s, void *ptr)
{
}

void *stack_pop(struct stack *s, int index)
{
}
