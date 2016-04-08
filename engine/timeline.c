/*
 * Cresus EVO - timeline_entry.c 
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/05/16
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 * 
 */

#include <stdlib.h>
#include <string.h>

#include "timeline.h"
#include "framework/verbose.h"

int timeline_init(struct timeline *t, const char *name, struct input *in) {
  
  /* Inheritance */
  __slist_super__(t);
  /* Data */
  strncpy(t->name, name, sizeof(t->name));
  t->ref = NULL;
  t->in = in;
  /* Internals */
  __list_head_init__(&t->list_entry);
  __slist_head_init__(&t->slist_indicator);
  
  return 0;
}

void timeline_release(struct timeline *t) {

  __list_head_release__(&t->list_entry);
  __slist_head_release__(&t->slist_indicator);
}

int timeline_add_indicator(struct timeline *t, struct indicator *i) {

  __slist_insert__(&t->slist_indicator, i);
}

int timeline_entry_next(struct timeline *t, struct timeline_entry **ret) {

  /* Is that function necessary ? */
  struct timeline_entry *entry;
  if((entry = input_read(t->in))){
    /* Cache data */
    t->ref = entry; /* Speed up things */
    /* Go out */
    *ret = entry;
    return 0;
  }
  
  return -1;
}

int timeline_entry_by_time(struct timeline *t, time_info_t time,
			   struct timeline_entry **ret) {

  do {
    /* Try to find a matching entry */
    struct timeline_entry *entry = t->ref;
    /* No init yet */
    if(!entry)
      continue;
    
    /* Granularity is provided by input entry, beware */
    int res = timeline_entry_timecmp(entry, time);
    if(!res){
      /* Cache data. Is that the right place ? */
      *ret = entry;
      return 1;
      
    }else{
      if(res < 0)
	/* We're late, let's move on */
	continue;
      
      else{
	/* Didn't find any with this timecode */
	return 0;
      }
    }
    
  }while((t->ref = input_read(t->in)));
  /* No more data */
  return -1;
}

struct timeline_entry *timeline_step(struct timeline *t,
				     struct timeline_entry *entry) {
  
  struct indicator *indicator;
  /* Add candle to list */
  list_add_tail(&t->list_entry, __list__(entry));
  /* And execute indicators */
  __slist_for_each__(&t->slist_indicator, indicator){
    indicator_feed(indicator, entry);
    /* PR_DBG("indicator %s\n", indicator->str); */
  }
  
  return entry;
}

int timeline_execute(struct timeline *t) {
  
  int n;
  struct timeline_entry *entry;
  
  for(n = 0;; n++){
    if(timeline_entry_next(t, &entry) != -1)
      timeline_step(t, entry);
    
    else
      break;
  }
  
  return n;
}

