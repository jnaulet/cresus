/*
 * Cresus EVO - mobile.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <stdio.h>

#include "mobile.h"

static int mobile_mma_test(void)
{
  struct mobile mma;
  mobile_init(&mma, MOBILE_MMA, 5, 1);
  
  for(int i = 1; i <= 5; i++)
    mobile_feed(&mma, i);
  
  printf("1-5 MMA is %.2lf\n", mobile_average(&mma));
  mobile_feed(&mma, 6);
  printf("2-6 MMA is %.2lf\n", mobile_average(&mma));
  mobile_feed(&mma, 7);
  printf("3-7 MMA is %.2lf\n", mobile_average(&mma));
  
  mobile_release(&mma);
  return 0;
}

static int mobile_ema_test(void)
{
  struct mobile ema;
  mobile_init(&ema, MOBILE_EMA, 5, 1);
  
  for(int i = 1; i <= 5; i++)
    mobile_feed(&ema, i);
  
  printf("1-5 EMA is %.2lf\n", mobile_average(&ema));
  mobile_feed(&ema, 6);
  printf("2-6 EMA is %.2lf\n", mobile_average(&ema));
  mobile_feed(&ema, 7);
  printf("3-7 EMA is %.2lf\n", mobile_average(&ema));
  
  mobile_release(&ema);
  return 0;
}

int main(int argc, char **argv)
{
  /* Mobile test */
  mobile_mma_test();
  mobile_ema_test();
  
  return 0;
}
