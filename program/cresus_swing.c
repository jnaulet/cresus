/*
 * Cresus EVO - cresus_swing.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>
#include <float.h>

#include "swing.h"
#include "yahoo.h"
#include "candle.h"

#define FILE "../csv/px1-2011-2013.csv"

int main(int argc, char **argv)
{
  struct yahoo y;
  struct candle cdl;
  struct swing swing;
  
  /* Misc */
  if(yahoo_init(&y, FILE) != -1 &&
     yahoo_read(&y, &cdl)){
    
    /* Init */
    swing_init(&swing, &cdl);
    
  }else /* Can't open file */
    return -1;

  for(;;){

    struct candle *res;

    /* We get next day data */
    if(!yahoo_read(&y, &cdl))
      break;
    
    if((res = swing_feed(&swing, &cdl)))
      printf("%s - %s swing type found at %.2lf (%.2lf)\n",
	     candle_localtime_str(res),
	     (swing.type == SWING_TOP ? "top" : "bottom"),
	     res->close, cdl.close);
  }
  
  /* Free all */
  yahoo_free(&y);
  return 0;
}
