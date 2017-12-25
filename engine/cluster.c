/*
 * Cresus EVO - cluster.c
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 22/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "candle.h"
#include "cluster.h"
#include "framework/verbose.h"

int cluster_init(struct cluster *c, const char *name, struct input *in,
		 time_info_t time_min, time_info_t time_max)
{  
  /* Super */
  __timeline_super__(c, name, in);
  slist_head_init(&c->slist_timeline);
  /* Set options */
  c->ref = &__timeline__(c)->list_entry;
  calendar_init(&c->cal, time_min, GRANULARITY_DAY); /* FIXME */
  c->time_max = time_max;

  return 0;
}

void cluster_release(struct cluster *c)
{
  __timeline_release__(c);
  slist_head_release(&c->slist_timeline);

  c->ref = NULL;
  calendar_release(&c->cal);
}

int cluster_add_timeline(struct cluster *c, struct timeline *t)
{
  __slist_insert__(&c->slist_timeline, t);
  return 0;
}

static void cluster_delete_by_time(struct cluster *c, time_info_t time)
{  
  struct timeline *t;
  __slist_for_each__(&c->slist_timeline, t){
    struct timeline_entry *entry;
    if(timeline_entry_current(t, &entry) != -1){
      if((entry = timeline_entry_find(entry, time)))
	timeline_trim_entry(t, entry);
    }
  }
}

static int cluster_prepare_step(struct cluster *c, time_info_t time,
				struct timeline_entry **ret)
{  
  int res;
  char buf[256]; /* debug */
  struct timeline *t;
  struct candle *candle;
  
  if(!candle_alloc(candle, time, calendar_granularity(&c->cal),
		   0, 0, 0, 0, 0)){
    /* Error */
    PR_ERR("can't allocate candle %s\n",
	   time2str(time, calendar_granularity(&c->cal), buf));
    /* TODO : add some kind of errno */
    return -1;
  }
  
  __slist_for_each__(&c->slist_timeline, t){
    struct timeline_entry *entry;
    /* Why not use granularity here to merge candles in timeline object ? */
    if((res = timeline_entry_by_time(t, time, &entry)) <= 0){
      PR_WARN("not enough data available in %s for %s\n", t->name,
	      time2str(time, calendar_granularity(&c->cal), buf));

      /* Remove any data that could have been copied anyway */
      candle_free(candle);
      cluster_delete_by_time(c, time);
      return res; /* EOF or 0 (no data available) */
      
    }else{
      /* Add entry in timeline */
      timeline_append_entry(t, entry);
      /* Merge candles */
      candle_merge(candle, __timeline_entry_self__(entry));
      PR_INFO("added candle %s %s in cluster\n", t->name,
	      timeline_entry_str(entry);
    }
  }

  /* Add data to list */
  *ret = __timeline_entry__(candle);
  __timeline_append_entry__(c, __timeline_entry__(candle));
  
  return 1;
}

static int cluster_step_ref(struct cluster *c,
			    struct timeline_entry **ret)
{  
  int res;
  char buf[256]; /* debug */

  struct timeline *t;
  struct timeline_entry *e;
  
  /* No calendar, using ref */
  if(timeline_entry_next(__timeline__(c), &e) < 0)
    /* No more data */
    return -1;

  /* Add data to our own list */
  timeline_append_entry(__timeline__(c), e);
  timeline_step(__timeline__(c));
  
  /* Parse internal list */
  __slist_for_each__(&c->slist_timeline, t){
    struct timeline_entry *entry;
    if((res = timeline_entry_by_time(t, e->time, &entry)) <= 0){
      PR_WARN("not enough data available in %s for %s\n", t->name,
	      time2str(e->time, e->granularity, buf));
      
      /* Reset indicators for this entry that has a problem */
      /* TODO : How to signal we have to ignore taken positions ? */
      timeline_reset_indicators(t);
      continue;
    }
    
    /* Add entry in timeline */
    timeline_append_entry(t, entry);
    timeline_step(t);
  }
  
  *ret = e; /* TODO : remove this */
  return 1;
}

static int cluster_execute_step(struct cluster *c)
{
  struct timeline *t;

  /* Don't forget that we inherit from timeline */
  timeline_step(__timeline__(c));
  
  /* Execute indicators */
  __slist_for_each__(&c->slist_timeline, t){
    /* Step all timelines */
    timeline_step(t);
    /* TODO : check return ? */
  }

  return 0;
}

int cluster_step(struct cluster *c)
{
  int ret;
  struct timeline_entry *entry;
  time_info_t time = calendar_time(&c->cal);

  if(__timeline__(c)->in != NULL){
    /* Ref acts as a calendar */
    if((ret = cluster_step_ref(c, &entry)) < 0){
      PR_ERR("no more data in ref input(s)\n");
      goto out;
    }
    
  }else{
    /* No ref. 
     * Using calendar & loop-read to ignore missing data */
    do {
      ret = cluster_prepare_step(c, time, &entry);
      calendar_next(&c->cal, &time);
    }while(!ret);
    
    if(ret < 0){ /* EOF */
      PR_ERR("no more data in input(s)\n");
      goto out;
    }
    
    /* Execute when possible */
    ret = cluster_execute_step(c);
  }
  
 out:
  return ret;
}

