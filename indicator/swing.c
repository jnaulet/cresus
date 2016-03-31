#include "swing.h"
#include <string.h>

#define __SWING_BACK(n) \
  &s->pool[((s->index - n) + SWING_MAX) % SWING_MAX] 

static swing_t __swing_detect(struct swing *s)
{
  if(s->count > SWING_MAX){
   
    struct candle *s0 = __SWING_BACK(2);
    struct candle *s1 = __SWING_BACK(1);
    struct candle *s2 = __SWING_BACK(0);

    if(s0->low < s1->low && s2->low < s1->low &&
       s0->high < s1->high && s2->high < s1->high)
      return SWING_TOP;

    if(s0->low > s1->low && s2->low > s1->low &&
       s0->high > s1->high && s2->high > s1->high)
      return SWING_BOTTOM;
  }

  return SWING_NONE;
}

int swing_init(struct swing *s, const struct candle *c)
{
  /* Super() */
  indicator_init(&s->parent, CANDLE_CLOSE, swing_feed);
  
  s->index = 0;
  s->count = 1;
  s->type = SWING_NONE;

  memcpy(&s->pool[s->index], c, sizeof *c);
  return 0;
}

void swing_free(struct swing *s)
{
  indicator_free(&s->parent);
}

int swing_feed(struct indicator *i, const struct candle *c)
{
  struct swing *s = (struct swing*)i;
  struct candle *cur = &s->pool[s->index];

  if(c->high > cur->high ||
     c->low < cur->low){
    /* Inc */
    s->count++;
    s->index = (s->index + 1) % SWING_MAX;
    memcpy(&s->pool[s->index], c, sizeof *c);

    if((s->type = __swing_detect(s)) != SWING_NONE)
      return cur;
  }
  
  return NULL;
}

swing_t swing_type_get(struct swing *s)
{
  return s->type;
}
