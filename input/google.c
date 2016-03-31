#include <string.h>
#include <stdlib.h>

#include "google.h"

static int __google_read(struct google *g, char *str,
                         struct candle *cdl)
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
  char *time = strsep(&str, ",");
  char *close = strsep(&str, ",");
  char *hi = strsep(&str, ",");
  char *lo = strsep(&str, ",");
  char *open = strsep(&str, ",");
  char *vol = str; /* End */
  
  /* Analysis */
  if(new_day) sscanf(time, "%d", &g->parser.time);
  else sscanf(time, "%d", &off);
  
  /* FIXME : check interval_s */
  cdl->timestamp = g->parser.time + (off * g->parser.interval_s);
  cdl->offset = g->parser.timezone_m;
  
  /* Set values */
  sscanf(open, "%lf", &cdl->open);
  sscanf(close, "%lf", &cdl->close);
  sscanf(hi, "%lf", &cdl->high);
  sscanf(lo, "%lf", &cdl->low);
  sscanf(vol, "%lf", &cdl->volume);
  
  if(new_day){
    cdl->type = CANDLE_SOD;
    fprintf(stderr, "--- Parsed new day at %s ---\n",
            candle_localtime_str(cdl, buf, sizeof buf));
    
  }else
    cdl->type = CANDLE_OTHER;
  
  return sizeof(*cdl);
}

static int google_input_read(struct input *in, struct candle *candle) {
  
  int ret;
  
  do
    ret = google_read((struct google*)in, candle, 1);
  while(ret == -1);
  
  return ret;
}

int google_init(struct google *g, const char *filename)
{
  input_init(&g->parent, google_input_read);
  
  if(!(g->fp = fopen(filename, "r")))
    return -1;
  
  return 0;
}

void google_free(struct google *g)
{
  input_free(&g->parent);
  if(g->fp) fclose(g->fp);
}

int google_read(struct google *g, struct candle *cdl, size_t n)
{
# define BUFLEN 256
  char buf[BUFLEN];
  struct candle tmp;
  
  /* Set hi/lo values */
  cdl->high = 0.0;
  cdl->open = 0.0;
  cdl->close = 0.0;
  cdl->low = DBL_MAX;
  cdl->volume = 0.0;
  
  for(int i = (int)n; i--;){
    
    if(!fgets(buf, sizeof buf, g->fp))
    /* End of file */
      return 0;
    
    if(__google_read(g, buf, &tmp) != -1){
      /* Get values */
      cdl->high = (tmp.high > cdl->high ? tmp.high : cdl->high);
      cdl->low = (tmp.low < cdl->low ? tmp.low : cdl->low);
      cdl->volume += tmp.volume;
      
      /* First element */
      if(i == (n - 1)){
        cdl->open = tmp.open;
        cdl->type = tmp.type;
      }
      /* Last element */
      if(!i){
        cdl->close = tmp.close;
        cdl->timestamp = tmp.timestamp;
        cdl->offset = tmp.offset;
      }
      
    }else
      return -1;
  }
  
  return (int)n;
}
