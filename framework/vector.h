#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <types.h>

struct vector {
  void **ptr;
  size_t size;
  size_t inc_size;
  /* current position */
  int pos;
};

static inline int vector_init(struct vector *ctx, size_t size)
{
  if((ctx->ptr = calloc(size, sizeof(*ctx->ptr))) != NULL){
    ctx->size = size;
    ctx->inc_size = size;
    ctx->pos = 0;
    return 0;
  }
  
  return -1;
}

static inline void vector_release(struct vector *ctx)
{
  free(ctx->ptr);
}

static inline int vector_add(struct vector *ctx, void *n3)
{
  ctx->ptr[ctx->pos++] = n3;
  if(ctx->pos == ctx->size){
    size_t new_size = ctx->size + ctx->inc_size;
    if(realloc(ctx->ptr, sizeof(*ctx->ptr) * new_size) == NULL)
      return -1;

    ctx->size = new_size;
  }
  
  return 0;
}

#define vector_for_each(ctx, n3)		\
  for(int i = 0, n3 = *ctx->ptr; i < ctx->size;	\
      i++, n3 = ctx->ptr[i])

#endif
