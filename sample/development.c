/*
 * Cresus EVO - development.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 07/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 *
 * Development sample code
 * Used to determine if interfaces are coherent for the "user"
 * And maybe to implement some kind of script commands 
 *
 * Not usable / does not compile anyway
 *
 */

#include <stdio.h>
#include <getopt.h>

#include "input/yahoo.h"
#include "engine/timeline.h"
#include "indicator/macd.h"
#include "indicator/mobile.h"
#include "indicator/rs_mansfield.h"

int main(int argc, char **argv) {

  /*
   * Data
   */
  
  /* Load / filter ref data */
  struct yahoo ref;
  yahoo_init(&ref, "data/FCHI.yahoo");

  /*
   * Timeline object
   */
  struct timeline timeline;
  timeline_init(&timeline);

  /* 
   * Indicators
   */
  
  /* Set mobile averages */
  struct mobile ema40, ema14, ema5;
  mobile_init(&ema40, MOBILE_EMA, 40, CANDLE_CLOSE);
  mobile_init(&ema14, MOBILE_EMA, 14, CANDLE_CLOSE);
  mobile_init(&ema5, MOBILE_EMA, 5, CANDLE_CLOSE);
  
  /* MACD */
  struct macd macd;
  macd_init(&macd, 12, 26, 9);
  
  /* RS mansfield */
  struct rs_mansfield rsm;
  rs_mansfield_init(&rsm, 14, timeline_entries(&timeline));

  /* Add all these indicators */
  timeline_add_indicator(&timeline, __indicator__(&ema40));
  timeline_add_indicator(&timeline, __indicator__(&ema14));
  timeline_add_indicator(&timeline, __indicator__(&ema5));
  timeline_add_indicator(&timeline, __indicator__(&macd));
  timeline_add_indicator(&timeline, __indicator__(&rsm));
  
  /* Load full data. FIXME : what about filtering ? */
  timeline_load(&timeline, __input__(&ref));

  /* TODO : step loop ? */
  
  return 0;
}
