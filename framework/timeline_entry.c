#include "timeline_entry.h"

int timeline_entry_init(struct timeline_entry *e, time_info_t time,
			granularity_t g) {
  
  __list_super__(e);
  
  e->time = time;
  e->granularity = g;
  
  return 0;
}

void timeline_entry_release(struct timeline_entry *e) {
  
  __list_release__(e);
}

int timeline_entry_timecmp(struct timeline_entry *e, time_info_t time) {
  
  return TIMECMP(e->time, time, e->granularity);
}

static struct timeline_entry *
timeline_entry_find_forward(struct timeline_entry *e, time_info_t time) {

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
timeline_entry_find_backwards(struct timeline_entry *e, time_info_t time) {

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
timeline_entry_find(struct timeline_entry *e, time_info_t time) {
  
  int tm = timeline_entry_timecmp(e, time);
  if(!tm)
    /* time is the same */
    goto out;
  
  if(tm < 0)
    return timeline_entry_find_forward(e, time);
  /* else */
  return timeline_entry_find_backwards(e, time);
  
 out:
  return e;
}

/* Debug functions */
const char *timeline_entry_str(struct timeline_entry *e,
			       char *buf, size_t len) {

  /* TODO : use granularity */
  snprintf(buf, len, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d ::%.3d",
	   TIME_GET_MONTH(e->time),
	   TIME_GET_DAY(e->time),
	   TIME_GET_YEAR(e->time),
	   TIME_GET_HOUR(e->time),
	   TIME_GET_MINUTE(e->time),
	   TIME_GET_SECOND(e->time),
	   TIME_GET_MSEC(e->time));
  
  return buf;
}
