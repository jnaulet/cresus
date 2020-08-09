/*
 * Cresus unit test - track.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08/11/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include "framework/plist.h"
#include "framework/verbose.h"
#include "framework/timeline_v2.h"

int main(int argc, char **argv)
{
  struct plist *p;
  char str[256], str2[256];
  struct timeline_v2 timeline;
  
  /* Timeline only */
  timeline_v2_init_ex(&timeline, argc, argv, NULL);
  
  /* Display info */
  struct track *t = timeline.by_track.next->ptr;
  plist_for_each(&t->plist_track_n3s, p){
    struct track_n3 *n3 = p->ptr;
    for(int i = QUARTERLY; i < PERIOD_MAX; i++){
      if(n3->quotes && n3->cash_flow.period[i])
        PR_INFO("%s: %s (%s)\n",
                time64_str_r(n3->quotes->time, GR_DAY, str),
                time64_str_r(n3->cash_flow.period[i]->time, GR_DAY, str2),
                i == QUARTERLY ? "quarterly" : "yearly");
    }
  }
}
