#include <string.h>
#include <stdlib.h>

#include "yahoo.h"

int yahoo_init(struct yahoo *y, const char *filename)
{
  /* super */
  input_init(&y->parent, yahoo_read);
  
  if(!(y->fp = fopen(filename, "r")))
    return -1;

  return 0;
}

void yahoo_free(struct yahoo *y)
{
  if(y->fp) fclose(y->fp);
}

int yahoo_read(struct input *in, struct candle *candle)
{
  char buf[256];
  char *str = buf;

  struct tm tm;
  int year, month, day;
  
  struct yahoo *y = (struct yahoo*)in;

  if(!fgets(buf, sizeof buf, y->fp))
    /* End of file */
    return 0;
  
  /* Cut string */
  char *time = strsep(&str, ",");
  char *open = strsep(&str, ",");
  char *hi = strsep(&str, ",");
  char *lo = strsep(&str, ",");
  char *close = strsep(&str, ",");
  char *vol = strsep(&str, ","); /* End */
  
  /* Set values */
  sscanf(time, "%d-%d-%d", &year, &month, &day);
  sscanf(open, "%lf", &candle->open);
  sscanf(close, "%lf", &candle->close);
  sscanf(hi, "%lf", &candle->high);
  sscanf(lo, "%lf", &candle->low);
  sscanf(vol, "%lf", &candle->volume); /* FIXME */
  
  /* TODO : Set EOD timestamp */
  memset(&tm, 0, sizeof tm);
  tm.tm_min = 30;
  tm.tm_hour = 17;
  tm.tm_mday = day;
  tm.tm_mon = month - 1;
  tm.tm_year = year - 1900;

  candle->timestamp = mktime(&tm);
  candle->offset = 0;

  return 1;
}
