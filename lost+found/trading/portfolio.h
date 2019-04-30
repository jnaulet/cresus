/*
 * Cresus EVO - portfolio.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#define PORTFOLIO_MAX 1

typedef enum {
  PORTFOLIO_LONG,
  PORTFOLIO_SHORT
} portfolio_t;

struct portfolio {
  int index;

  struct {
    double value;
    portfolio_t type;
  } n3[PORTFOLIO_MAX];
};

int portfolio_init(struct portfolio *p); /* TODO : define size here */
void portfolio_release(struct portfolio *p);

double portfolio_buy(struct portfolio *p, portfolio_t type, double value);
double portfolio_sell(struct portfolio *p, double value);
double portfolio_sell_all(struct portfolio *p, double value);


#endif
