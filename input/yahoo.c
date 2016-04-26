/*
 * Cresus EVO - yahoo.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>

#include "yahoo.h"
#include "engine/candle.h"
#include "framework/verbose.h"

static struct timeline_entry *yahoo_read(struct input *in) {
  
  struct yahoo *y = __input_self__(in);
  
  y->current_entry = y->list_entry.next;
  if(list_is_head(y->current_entry))
    return NULL; /* EOF */
  
  /* Don't forget we'll put it in another list */
  list_del(y->current_entry);
  return __list_self__(y->current_entry);
}

static int yahoo_load_entry(struct yahoo *y,
			    struct timeline_entry **ret,
			    time_info_t time_min, /* FIXME */
			    time_info_t time_max) {
  
  char buf[256];
  char *str = buf;
  struct candle *candle = NULL;

  time_info_t time = 0;
  int year, month, day;
  double open, close, high, low, volume;
  
  if(!fgets(buf, sizeof buf, y->fp))
    /* End of file */
    return -1;
  
  /* Cut string */
  char *stime = strsep(&str, ",");
  char *sopen = strsep(&str, ",");
  char *shi = strsep(&str, ",");
  char *slo = strsep(&str, ",");
  char *sclose = strsep(&str, ",");
  char *svol = strsep(&str, ","); /* End */
  
  /* Set values */
  sscanf(stime, "%d-%d-%d", &year, &month, &day);
  sscanf(sopen, "%lf", &open);
  sscanf(sclose, "%lf", &close);
  sscanf(shi, "%lf", &high);
  sscanf(slo, "%lf", &low);
  sscanf(svol, "%lf", &volume);

  /* Dummy values for control */
  TIME_SET_SECOND(time, 1);
  TIME_SET_MINUTE(time, 30);
  TIME_SET_HOUR(time, 17);
  
  TIME_SET_DAY(time, day);
  TIME_SET_MONTH(time, month);
  TIME_SET_YEAR(time, year);
  
  /* No intraday on yahoo */
  if(TIMECMP(time, time_min, GRANULARITY_DAY) >= 0 &&
     TIMECMP(time, time_max, GRANULARITY_DAY) <= 0){
    /* Create candle (at last !) */
    if(candle_alloc(candle, time, GRANULARITY_DAY,
		    open, close, high, low, volume)){
      /* Candle is allocated & init */
      *ret = __timeline_entry__(candle);
      PR_DBG("%s %.2d/%.2d/%.4d loaded\n", y->filename,
	     TIME_GET_MONTH(time), TIME_GET_DAY(time), TIME_GET_YEAR(time));
      
      return 1;
    }
    
  }else
    return 0;
  
  return -1;
}

static int yahoo_load(struct yahoo *y) {
  
  int n;
  char info[256];
  struct timeline_entry *entry;
  
  /* Yahoo is LIFO with first line showing format */
  fgets(info, sizeof info, y->fp);
  
  for(n = 0;;){
    switch(yahoo_load_entry(y, &entry,
			    __input__(y)->from,
			    __input__(y)->to)) {
    case 0 : break;
    case -1 : goto out;
    default :
      __list_add__(&y->list_entry, entry);
      n++;
      break;
    }
  }
  
 out:
  return n;
}

int yahoo_init(struct yahoo *y, const char *filename,
	       time_info_t from, time_info_t to) {
  
  /* super */
  __input_super__(y, yahoo_read, from, to);
  list_head_init(&y->list_entry);
  
  strncpy(y->filename, filename, sizeof(y->filename));
  if(!(y->fp = fopen(y->filename, "r"))){
    PR_ERR("unable to open file %s\n", y->filename);
    return -1;
  }
  
  /* Load all data */
  yahoo_load(y);
  return 0;
}

void yahoo_release(struct yahoo *y) {
  
  __input_release__(y);
  if(y->fp) fclose(y->fp);
}
