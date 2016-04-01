/*
 * Cresus EVO - cresus_vp.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "google.h"
#include "candle.h"
#include "statistics.h"

#define DATA ""

struct volume_price {

  struct google google;

#define VP_MAX 3
#define VP_BACK(vp, n)					\
  &(vp)->pool[(((vp)->index - n) + VP_MAX) % VP_MAX]

  int index, count;
  struct candle pool[VP_MAX];
};

#define VP_PRICE_UP    (1 << 0)
#define VP_PRICE_DOWN  (1 << 1)
#define VP_VOLUME_UP   (1 << 2)
#define VP_VOLUME_DOWN (1 << 3)

static int volume_price_detect(struct volume_price *vp)
{
  int ret = 0;

  if(vp->count > VP_MAX){
    
    struct candle *c0 = VP_BACK(vp, 2);
    struct candle *c1 = VP_BACK(vp, 1);
    struct candle *c2 = VP_BACK(vp, 0);

    if(c1->close > c0->close && c1->close > c2->close)
      ret |= VP_PRICE_UP;

    if(c1->close < c0->close && c1->close < c2->close)
      ret |= VP_PRICE_DOWN;

    if(c1->volume > c0->volume && c1->volume > c2->volume)
      ret |= VP_VOLUME_UP;

    if(c1->volume < c0->volume && c1->volume < c2->volume)
      ret |= VP_VOLUME_DOWN;
  }
  
  return ret;
}

static int volume_price_feed(struct volume_price *vp,
			     struct candle *cdl)
{
  vp->count++;
  memcpy(&vp->pool[vp->index], cdl, sizeof *cdl);
  vp->index = (vp->index + 1) % VP_MAX;

  return volume_price_detect(vp);
}

int volume_price_init(struct volume_price *vp)
{
  struct candle cdl;

  if(google_init(&vp->google, "/dev/stdin") < 0)
    return -1;

  if(google_read(&vp->google, &cdl, 1) <= 0)
    return -1;

  /* init pool */
  vp->index = 0;
  vp->count = 0;
  volume_price_feed(vp, &cdl);

  return 0;
}

void volume_price_free(struct volume_price *vp)
{
  google_free(&vp->google);
}

int volume_price_run(struct volume_price *vp)
{
  int status;
  struct candle cdl;

  /* Read candle */
  while(google_read(&vp->google, &cdl, 1) > 0){
    /* Try to find ups & downs */
    if((status = volume_price_feed(vp, &cdl))){
      if(status & VP_PRICE_UP) printf("VP_PRICE_UP, ");
      if(status & VP_PRICE_DOWN) printf("VP_PRICE_DOWN, ");
      if(status & VP_VOLUME_UP) printf("VP_VOLUME_UP, ");
      if(status & VP_VOLUME_DOWN) printf("VP_VOLUME_DOWN, ");
      
      printf("at %s (0x%04x) %d\n", candle_localtime_str(&cdl),
	     status, vp->count);
    }
  }

  return -1;
}

int main(int argc, char **argv)
{
  struct volume_price vp;

  volume_price_init(&vp);
  volume_price_run(&vp);

  return 0;
}
