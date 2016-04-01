/*
 * Cresus EVO - cresus_ex.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rsi.h"
#include "macd.h"
#include "yahoo.h"
#include "candle.h"
#include "mobile.h"
#include "bollinger.h"
#include "statistics.h"

#define DATA "../csv/px1-historical.csv"

/* EMAs */
#define EMA_P 6
#define MMA_P 6
/* Bollinger */
#define BOLLINGER_P   14
#define BOLLINGER_DEV 2.0
/* RSI */
#define RSI_P 12
/* MACD ? */

/* Stoploss */
//#define STOPLOSS 15
//#define REBUY

struct cresus_ex {

  struct yahoo yahoo;
  struct rsi rsi;
  struct macd macd;
  struct mobile mma, ema;
  struct bollinger boll;
  struct statistics stats;

  double last_dir_slow, last_dir_fast;
  double last_close;

  int right, wrong;
  int white, black;
};

int cresus_ex_init(struct cresus_ex *c)
{
  struct candle cdl;

  if(yahoo_init(&c->yahoo, DATA) < 0)
    return -1;
  
  /* Read first item */
  if(yahoo_read(&c->yahoo, &cdl) < 0)
    return -1;

  /* MAs */
  mobile_init(&c->mma, MOBILE_MMA, MMA_P, cdl.close);
  mobile_init(&c->ema, MOBILE_EMA, EMA_P, cdl.close);
  /* Bollinger */
  bollinger_init(&c->boll, BOLLINGER_P, BOLLINGER_DEV, &cdl);
  /* RSI */
  rsi_init(&c->rsi, RSI_P, &cdl);
  /* Stats */
  statistics_init(&c->stats, 0);
  
  /* Directions */
  c->last_dir_slow = 0.0;
  c->last_dir_fast = 0.0;

  /* Gap */
  c->last_close = cdl.close;

  /* Mizc */
  c->right = 0;
  c->wrong = 0;
  c->white = 0;
  c->black = 0;

  /* Pseudo-random */
  srandom(time(NULL));
  
  return 0;
}

void cresus_ex_free(struct cresus_ex *c)
{
  yahoo_free(&c->yahoo);

  rsi_free(&c->rsi);
  //macd_free(&c->macd);
  mobile_free(&c->mma);
  mobile_free(&c->ema);
  bollinger_free(&c->boll);

  statistics_printf(&c->stats);
  statistics_free(&c->stats);

  printf("\n%d right, %d wrong (%.2lf%%), total %d\n", c->right, c->wrong,
	 ((double)c->right / (c->right + c->wrong)) * 100.0,
	 c->right + c->wrong);
  printf("%d white, %d black (%.2lf%%), total %d\n\n", c->white, c->black,
	 ((double)c->white / (c->white + c->black)) * 100.0,
	 c->black + c->white);
}

void cresus_ex_op(struct cresus_ex *c, struct candle *cdl)
{
  double op = 0.0;

  if(c->last_dir_fast > 0.0){
    /* EMA is up */
    op = cdl->close - cdl->open;

#ifdef STOPLOSS
    if(cdl->low < cdl->open - STOPLOSS){
      op = -STOPLOSS;
#  ifdef REBUY
      /* if STOP hit, re-buy in opposite direction */
      op += -STOPLOSS + (cdl->open - cdl->close);
#  endif
    }
#endif
    
    /* Stats */
    statistics_accumulate(&c->stats, op);
    
  }else if(c->last_dir_fast < 0.0){
    /* EMA is down */
    op = cdl->open - cdl->close;

#ifdef STOPLOSS
    if(cdl->high > cdl->open + STOPLOSS){
      op = -STOPLOSS;

#  ifdef REBUY
      /* if STOP hit, re-buy in opposite direction */
      op += -STOPLOSS + (cdl->close - cdl->open);
#  endif
    }
#endif

    /* Stats */
    statistics_accumulate(&c->stats, op);
    
  }else{
    /* Direction is 0.O */
    printf("%s - no op\n", candle_localtime_str(cdl));
  }

  /* printf("%s,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf\n", */
  /* 	   cdl->date, cdl->open, cdl->close, cdl->high, */
  /* 	   cdl->close, mobile_average(&c->ema)); */

  if(op > 0) c->right++;
  if(op < 0) c->wrong++;

  if(cdl->open < cdl->close) c->white++;
  else /* if(cdl->open > cdl->close) */c->black++;
}

int cresus_ex_run(struct cresus_ex *c)
{
  struct candle cdl;
  static int count = 0;
  static int state = 0;

  /* START OF DAY */

  /* Read next element */
  if(!yahoo_read(&c->yahoo, &cdl))
    return -1;

  /* Gap algorithm */
  /* c->last_dir_fast = (cdl.open > c->last_close ? 1 : -1); */
  /* c->last_close = cdl.close; */

  /* OP */
  cresus_ex_op(c, &cdl);

  /* END OF DAY */

  /* Ema algorithm */
  /* double ema = mobile_average(&c->ema); */
  /* c->last_dir_fast = ema - mobile_feed(&c->ema, cdl.close); */

  /* Mma algorithm */
  /* double mma = mobile_average(&c->mma); */
  /* c->last_dir_fast = mma - mobile_feed(&c->mma, cdl.close); */

  /* 1/2 algorithm */
  /* if(count++ % 2) c->last_dir_fast = 1.0; */
  /* else c->last_dir_fast = -1.0; */

  /* Blackjack algorithm */
/* #define BJMAX 32 */
/*   count += ((cdl.close > cdl.open) ? 1 : -1); */
/*   count = (count > BJMAX ? BJMAX : count); */
/*   count = (count < -BJMAX ? -BJMAX : count); */
/*   c->last_dir_fast = cdl.open - cdl.close; */

/*   c->last_dir_fast = (count > (BJMAX/1.4) ? -1 : c->last_dir_fast); */
/*   c->last_dir_fast = (count < (-BJMAX/1.4) ? 1 : c->last_dir_fast); */
  
/*   printf("Count %d\n", count); */

  /* All white algorithm */
  /* c->last_dir_fast = 1; */

  /* Different from today algorithm */
  c->last_dir_fast = cdl.open - cdl.close;

  /* Random algorithm */
  /* if(random() % 2) c->last_dir_fast = 1; */
  /* else c->last_dir_fast = -1; */

  /* Weighted algorithm */
  rsi_feed(&c->rsi, &cdl);
  if(rsi_compute(&c->rsi) >= 70.0) c->last_dir_fast = -1.0;
  if(rsi_compute(&c->rsi) <= 35.0) c->last_dir_fast = 1.0;
  
  //printf("rsi %.2lf\n", rsi_compute(&c->rsi));
  //c->last_dir_fast = (rsi_value(&c->rsi) >= 0 ? -1 : 1);
  
  /* Bollinger algorithm */
  if(bollinger_feed(&c->boll, &cdl)){
    if(cdl.high >= bollinger_compute(&c->boll)->hi) c->last_dir_fast = -1;
    if(cdl.low >= bollinger_compute(&c->boll)->lo) c->last_dir_fast = -1;
  }

  /* TODO ? : Use longer seed to weight decisions */
  /* TODO ? : Use momentum or volatility index */

  return 0;
}

int main(int argc, char **argv)
{
  struct cresus_ex cresus_ex;

  /* Init */
  if(cresus_ex_init(&cresus_ex) < 0)
    return -1;

  while(cresus_ex_run(&cresus_ex) != -1)
    /* Nop */;

  cresus_ex_free(&cresus_ex);
  return 0;
}
