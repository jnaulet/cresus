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
  } entry[PORTFOLIO_MAX];
};

int portfolio_init(struct portfolio *p); /* TODO : define size here */
void portfolio_free(struct portfolio *p);

double portfolio_buy(struct portfolio *p, portfolio_t type, double value);
double portfolio_sell(struct portfolio *p, double value);
double portfolio_sell_all(struct portfolio *p, double value);


#endif