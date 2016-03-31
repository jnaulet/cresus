#include <math.h>
#include <string.h>

#include "zigzag.h"

int zigzag_init(struct zigzag *z, zigzag_t type, double thres,
                candle_value_t value,
                const struct candle *seed)
{
  /* Super */
  indicator_init(&z->parent, value, zigzag_feed);
  
  z->type = type;
  z->threshold = (type == ZIGZAG_PERCENT ? thres / 100.0 : thres);

  /* Refs */
  memcpy(&z->ref, seed, sizeof *seed);
  memcpy(&z->last_ref, seed, sizeof *seed);

  z->ref_count = 0;
  z->direction = ZIGZAG_DIR_NONE;

  return 0;
}

void zigzag_free(struct zigzag *z)
{
  indicator_free(&z->parent);
  z->ref_count = 0;
  z->direction = ZIGZAG_DIR_NONE;
}

static void zigzag_ref_save(struct zigzag *z)
{
  memcpy(&z->last_ref, &z->ref, sizeof z->last_ref);
}

static void zigzag_ref_set(struct zigzag *z, const struct candle *candle)
{
  memcpy(&z->ref, candle, sizeof *candle);
  z->ref_count = 0;
}

int zigzag_feed(struct indicator *i, const struct candle *candle)
{
  struct zigzag *z = (struct zigzag*)i;
  
  double threshold;
  double candle_value = candle_get_value(candle, i->value);
  double ref_value = candle_get_value(&z->ref, i->value);
  double gap = candle_value - ref_value;

  if(z->type == ZIGZAG_PERCENT)
    threshold = z->threshold * ref_value;
  else /* ZIGZAG_ABSOLUTE */
    threshold = z->threshold;

  z->ref_count++; /* FIXME */

  switch(z->direction){
  case ZIGZAG_DIR_UP :
    /* Up-ing reference */
    if(candle_value > ref_value)
      zigzag_ref_set(z, candle);
    
    /* Changing direction */
    if(gap <= -(threshold)){
      zigzag_ref_save(z);
      z->direction = ZIGZAG_DIR_DOWN;
      zigzag_ref_set(z, candle);
      /* Throw event */
      /* indicator_throw_event(i, EVENT_ZIGZAG_CHDIR, candle); */
    }
    break;
    
  case ZIGZAG_DIR_DOWN :
    /* Down-ing reference */
    if(candle_value < ref_value)
      zigzag_ref_set(z, candle);
    
    /* Changing direction */
    if(gap >= threshold){
      zigzag_ref_save(z);
      z->direction = ZIGZAG_DIR_UP;
      zigzag_ref_set(z, candle);
      /* Throw event */
      /* indicator_throw_event(i, EVENT_ZIGZAG_CHDIR, candle); */
    }
    break;
    
  case ZIGZAG_DIR_NONE :
    if(fabs(gap) >= threshold){
      z->direction = (gap > 0 ? ZIGZAG_DIR_UP : ZIGZAG_DIR_DOWN);
      zigzag_ref_set(z, candle);
      /* Throw event */
      /* indicator_throw_event(i, EVENT_ZIGZAG_CHDIR, candle); */
    }
    break;
  }
  
  return z->direction;
}

zigzag_dir_t zigzag_get_last_ref(struct zigzag *z, struct candle *last_ref)
{
  memcpy(last_ref, &z->last_ref, sizeof *last_ref);
  return z->direction;
}

