/*
 * Cresus EVO - statistics.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>
#include <float.h>

#include "statistics.h"

int statistics_init(struct statistics *s, double op_cost)
{
  s->gross_profit = 0.0;
  s->gross_loss = 0.0;
  s->total_op_cost = 0.0;
  s->total_net_profit = 0.0;
  s->total_number_of_trades = 0;
  s->number_winning_trades = 0;
  s->number_losing_trades = 0;
  s->max_consec_winners = 0;
  s->max_consec_losers = 0;
  s->largest_winning_trade = 0.0;
  s->largest_losing_trade = 0.0;
  s->average_winning_trade = 0.0;
  s->average_losing_trade = 0.0;
  s->drawdown = 0.0;
  s->profit_factor = 0.0;
  s->account_size_required = 0.0;
  s->return_on_account = 0.0;

  s->tmp.win = 0;
  s->tmp.loss = 0;
  s->tmp.max = 0.0;

  s->op_cost = op_cost;

  return 0;
}

void statistics_release(struct statistics *s)
{
}

void statistics_feed(struct statistics *s, double result)
{
  s->total_op_cost = (++s->total_number_of_trades) * s->op_cost;

  if(result > 0.0){
    s->gross_profit += result;
    s->largest_winning_trade = (result > s->largest_winning_trade ?
				result : s->largest_winning_trade);
    s->average_winning_trade = ((s->number_winning_trades *
				 s->average_winning_trade + result) /
				(s->number_winning_trades + 1));
    
    s->tmp.win++;
    s->tmp.loss = 0;
    s->max_consec_winners = (s->tmp.win > s->max_consec_winners ?
			     s->tmp.win : s->max_consec_winners);
    
    s->number_winning_trades++;

  }else{
    s->gross_loss += result;
    s->largest_losing_trade = (result < s->largest_losing_trade ? result :
			       s->largest_losing_trade);
    s->average_losing_trade = ((s->number_losing_trades *
				s->average_losing_trade + result) /
			       (s->number_losing_trades + 1));

    s->tmp.loss++;
    s->tmp.win = 0;
    s->max_consec_losers = (s->tmp.loss > s->max_consec_losers ?
			    s->tmp.loss : s->max_consec_losers);

    s->number_losing_trades++;
  }
  
  s->total_net_profit = s->gross_profit + s->gross_loss - s->total_op_cost;
  s->tmp.max = (s->total_net_profit > s->tmp.max ?
		s->total_net_profit : s->tmp.max);
  
  s->drawdown = ((s->total_net_profit - s->tmp.max) < s->drawdown ?
		 (s->total_net_profit - s->tmp.max) : s->drawdown);

  s->profit_factor = s->gross_profit / fabs(s->gross_loss);
  s->account_size_required = fabs(s->drawdown);
  s->return_on_account = s->total_net_profit / s->account_size_required;
}

int statistics_printf(struct statistics *s)
{
  fprintf(stderr, "Gross profit          %+.2lf\n", s->gross_profit);
  fprintf(stderr, "Gross loss            %+.2lf\n", s->gross_loss);
  fprintf(stderr, "Operations cost       %+.2lf\n", s->total_op_cost);
  fprintf(stderr, "Total net profit      %+.2lf\n", s->total_net_profit);
  fprintf(stderr, "Total num of trades   %d\n",     s->total_number_of_trades);
  fprintf(stderr, "Num winning trades    %d\n",     s->number_winning_trades);
  fprintf(stderr, "Num losing trades     %d\n",     s->number_losing_trades);
  fprintf(stderr, "Max consec winners    %d\n",     s->max_consec_winners);
  fprintf(stderr, "Max consec losers     %d\n",     s->max_consec_losers);
  fprintf(stderr, "Largest winning trade %+.2lf\n", s->largest_winning_trade);
  fprintf(stderr, "Largest losing trade  %+.2lf\n", s->largest_losing_trade);
  fprintf(stderr, "Average winning trade %+.2lf\n", s->average_winning_trade);
  fprintf(stderr, "Average losing trade  %+.2lf\n", s->average_losing_trade);
  fprintf(stderr, "Drawdown              %+.2lf\n", s->drawdown);
  fprintf(stderr, "Profit factor         %.2lf\n",  s->profit_factor);
  fprintf(stderr, "Account size required %.2lf\n",  s->account_size_required);
  fprintf(stderr, "Return on account     %+.2lf\n", s->return_on_account);

  return 0;
}
