/*
 * Cresus EVO - timeline_entry.c
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/05/16
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include "timeline_entry.h"

int timeline_entry_init(struct timeline_entry *e, time_info_t time,
			granularity_t g)
{  
  __list_super__(e);
  
  e->time = time;
  e->granularity = g;
  
  return 0;
}

void timeline_entry_release(struct timeline_entry *e)
{  
  __list_release__(e);
}

time_info_t timeline_entry_timecmp(struct timeline_entry *e,
				   time_info_t time)
{  
  return TIMECMP(e->time, time, e->granularity);
}

static struct timeline_entry *
timeline_entry_find_forward(struct timeline_entry *e, time_info_t time)
{
  struct list *entry;
  for(entry = __list__(e)->next;
      entry != entry->head;
      entry = entry->next){
    /* Find forward */
    struct timeline_entry *cur = __list_self__(entry);
    if(!timeline_entry_timecmp(cur, time))
      return cur;
  }

  return NULL;
}

static struct timeline_entry *
timeline_entry_find_backwards(struct timeline_entry *e, time_info_t time)
{
  struct list *entry;
  for(entry = __list__(e)->prev;
      entry != entry->head;
      entry = entry->prev){
    /* Find forward */
    struct timeline_entry *cur = __list_self__(entry);
    if(!timeline_entry_timecmp(cur, time))
      return cur;
  }

  return NULL;
}

struct timeline_entry *
timeline_entry_find(struct timeline_entry *e, time_info_t time)
{  
  time_info_t cmp;
  if(!(cmp = timeline_entry_timecmp(e, time)))
    /* time is the same */
    goto out;
  
  if(cmp < 0)
    return timeline_entry_find_forward(e, time);
  /* else */
  return timeline_entry_find_backwards(e, time);
  
 out:
  return e;
}

/* Debug functions */

static char str[256];

const char *timeline_entry_str(struct timeline_entry *e)
{
  return timeline_entry_str_r(e, str, sizeof(str));
}

const char *timeline_entry_str_r(struct timeline_entry *e,
				 char *buf, size_t len)
{
  /* FIXME : use len */
  return time_info2str_r(e->time, e->granularity, buf);
}
