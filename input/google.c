/*
 * Cresus EVO - google.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>

#include "google.h"
#include "engine/candle.h"

static int __google_readline(struct google *g,
			     char *str, size_t size,
			     time_t *time,
			     double *open, double *close,
			     double *high, double *low,
			     double *volume)
{
  int off = 0;
  char buf[256];
  int new_day = 0;
  
  /* Set interval */
  if(!strncmp(str, "INTERVAL=", 9)){
    /* FIXME */
    sscanf(str + 9, "%d", &g->parser.interval_s);
    return -1;
  }
  
  /* Set timezone */
  if(!strncmp(str, "TIMEZONE_OFFSET=", 16)){
    /* FIXME */
    sscanf(str + 16, "%d", &g->parser.timezone_m);
    //printf("Timezone offset is %d\n", c->parser.timezone_m);
    return -1;
  }
  
  if(*str == 'a'){ /* New day */
    new_day = 1;
    str++;
    
  }else if(*str < '0' || *str > '9')
    return -1; /* No useful data */
  
  /* Cut string */
  char *stime = strsep(&str, ",");
  char *sclose = strsep(&str, ",");
  char *shi = strsep(&str, ",");
  char *slo = strsep(&str, ",");
  char *sopen = strsep(&str, ",");
  char *svol = str; /* End */
  
  /* Analysis */
  if(new_day) sscanf(stime, "%d", &g->parser.time);
  else sscanf(stime, "%d", &off);
  
  /* Set values */
  /* FIXME : check interval_s */
  *time = g->parser.time + (off * g->parser.interval_s);
  sscanf(sopen, "%lf", open);
  sscanf(sclose, "%lf", close);
  sscanf(shi, "%lf", high);
  sscanf(slo, "%lf", low);
  sscanf(svol, "%lf", volume);

  /* TODO : make some use of these flags
  if(new_day){
    cdl->type = CANDLE_SOD;
    fprintf(stderr, "--- Parsed new day at %s ---\n",
            __timeline_entry_localtime_str__(candle, buf, sizeof buf));
    
  }else
    cdl->type = CANDLE_OTHER;
  */
  
  return 1;
}

static int __google_read(struct google *g, struct candle *candle)
{
# define BUFLEN 256
  char buf[BUFLEN];

  time_t time;
  /* Set hi/lo values */
  double high = 0.0;
  double open = 0.0;
  double close = 0.0;
  double low = DBL_MAX;
  double volume = 0.0;

  for(;;){
    if(!fgets(buf, sizeof buf, g->fp))
      /* EOF */
      return 0;

    if(__google_readline(g, buf, sizeof buf, &time,
			 &open, &close, &high, &low, &volume) != -1){
      /* Set candle */
      candle_init(candle, time, GRANULARITY_DAY, /* FIXME : use parser_interval ? */
		  open, close, high, low, volume);
      break;
    }
  }
  
  return 1;
}

static int google_input_read(struct input *in,
			     struct timeline_entry *entry) {

  /* Is that small wrapping necessary */
  return __google_read(__input_self__(in),
		       __timeline_entry_self__(entry));
}

int google_init(struct google *g, const char *filename)
{
  /* super() */
  __input_super__(g, google_input_read);
  
  if(!(g->fp = fopen(filename, "r")))
    return -1;
  
  return 0;
}

void google_free(struct google *g)
{
  __input_free__(g);
  if(g->fp) fclose(g->fp);
}
