#include "timeline_entry.h"

#define __timeline_entry_time(time, granularity)	\
  (time - (time % granularity))

int timeline_entry_init(struct timeline_entry *e, time_t time,
			granularity_t g) {
  
  __list_super__(e);
  
  e->time = time;
  e->granularity = g;
  
  return 0;
}

void timeline_entry_free(struct timeline_entry *e) {
  
  __list_free__(e);
}

time_t timeline_entry_timecmp(struct timeline_entry *e, time_t time) {
  
  return (__timeline_entry_time(e->time, e->granularity) -
	  __timeline_entry_time(time, e->granularity));
}

struct timeline_entry *
timeline_entry_find(struct timeline_entry *e, time_t time) {
  
  struct timeline_entry *entry;
  time_t tm = timeline_entry_timecmp(e, time);
  
  if(!tm)
    /* time is the same */
    goto out;
  
  if(tm < 0){
    /* time is forward */
    __list_for_each__(__list__(e), entry)
      if(!timeline_entry_timecmp(entry, time))
	goto out;
    
  }else{
    /* time is backwards*/
    __list_for_each_prev__(__list__(e), entry)
      if(!timeline_entry_timecmp(entry, time))
	goto out;
  }
  
  return NULL;
  
 out:
  return entry;
}

/* Debug functions */
const char *timeline_entry_localtime_str(struct timeline_entry *e,
					 char *buf, size_t len) {
  struct tm tm;
  time_t time = e->time;
  
  strftime(buf, len, "%c", gmtime_r(&time, &tm));
  return buf;
}
