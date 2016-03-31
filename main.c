//
//  main.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 21/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include <stdio.h>

#include "yahoo.h"
#include "google.h"
#include "candle.h"
#include "engine.h"
#include "mobile.h"

#include "mod_mma_simple.h"
#include "mod_zigzag.h"
#include "mod_stoploss.h"
#include "mod_supres.h"
#include "mod_srsi.h"
#include "mod_ema_ha.h"

int main(int argc, char **argv) {
  
  struct yahoo yahoo;
  //struct google google;
  struct engine engine;
  struct mod_mma_simple mod_mma_simple;
  struct mod_zigzag mod_zigzag;
  struct mod_stoploss mod_stoploss;
  struct mod_supres mod_supres;
  struct mod_srsi mod_srsi;
  struct mod_ema_ha mod_ema_ha;
  
  struct input *input = (struct input*)&yahoo;
  //struct input *input = (struct input*)&google;
  
  struct candle seed;
  
  /* Init input */
  yahoo_init(&yahoo, "/Users/jnaulet/px1-historical.csv");
  //yahoo_init(&yahoo, "/Users/jnaulet/AF.csv");
  //yahoo_init(&yahoo, "/Users/jnaulet/ALU.csv");
  //yahoo_init(&yahoo, "/Users/jnaulet/GOOGL_eod.csv");
  //yahoo_read(&yahoo.parent, &seed);
  //google_init(&google, "/Users/jnaulet/GOOG.csv");
  //input_read(&google.parent, &seed);
  
  input_read(input, &seed);
  
  /* Init modules */
  mod_mma_simple_init(&mod_mma_simple, &seed);
  mod_zigzag_init(&mod_zigzag, &seed);
  mod_stoploss_init(&mod_stoploss, &seed);
  mod_supres_init(&mod_supres, &seed);
  mod_srsi_init(&mod_srsi, &seed);
  mod_ema_ha_init(&mod_ema_ha, &seed);
  
  /* Init engine */
  //engine_init(&engine, &yahoo.parent);
  engine_init(&engine, input);
  //engine_add_module(&engine, &mod_mma_simple.parent);
  engine_add_module(&engine, &mod_zigzag.parent);
  //engine_add_module(&engine, &mod_stoploss.parent);
  //engine_add_module(&engine, &mod_supres.parent);
  //engine_add_module(&engine, &mod_srsi.parent);
  //engine_add_module(&engine, &mod_srsi.parent);
  //engine_add_module(&engine, &mod_ema_ha.parent);
  
  /* Run engine */
  engine_run(&engine, -1);
  
  /* Free */
  engine_free(&engine);
  //mod_mma_simple_free(&mod_mma_simple);
  mod_zigzag_free(&mod_zigzag);
  //mod_stoploss_free(&mod_stoploss);
  //mod_supres_free(&mod_supres);
  //mod_srsi_free(&mod_srsi);
  //mod_ema_ha_free(&mod_ema_ha);
  
  //yahoo_free(&yahoo);
  //google_free(&google);
  
  /* The End */
  return 0;
}