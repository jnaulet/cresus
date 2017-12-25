/*
 * Cresus EVO - statistics.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef STATISTICS_H
#define STATISTICS_H

struct statistics {

  /* Import from tradestation */
  double gross_profit; /* Total positions gagnantes */
  double gross_loss; /* Total positions perdantes */
  double total_op_cost; /* Montant des frais */
  double total_net_profit; /* gross profit - gross loss */  
  int total_number_of_trades; /* Comme son nom l'indique */
  double percent_profitable; /* Nombre de positions gagnantes en % */
  int number_winning_trades;
  int number_losing_trades;
  double largest_losing_trade; /* Plus grosse perdante */
  double largest_winning_trade; /* Plus grosse gagnante */
  double average_winning_trade; /* Montant moyen des gains */
  double average_losing_trade; /* Montant moyen des pertes */
  int max_consec_winners; /* Nombre de gains consecutifs max */
  int max_consec_losers; /* Nombre de pertes consécutives max */
  double drawdown; /* Montant du plus fort recul de notre capital */
  double profit_factor; /* Gross profit / gross loss */
  double account_size_required; /* Gross-modo le drawdown */
  double return_on_account; /* Net profit / account size required */
 
  /* Temp value */
  struct {
    int loss, win;
    double max;
  } tmp;
  
  double op_cost;
};

int statistics_init(struct statistics *s, double op_cost);
void statistics_release(struct statistics *s);

/* API */
void statistics_feed(struct statistics *s, double result);
int statistics_printf(struct statistics *s);

#endif
