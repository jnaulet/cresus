#include "esi.h"

int esi_init(struct esi *e, mobile_t type, int smooth_p)
{
  e->last = 0;
  return mobile_init(&e->smooth, type, smooth_p, e->last);
}

void esi_free(struct esi *e)
{
  mobile_free(&e->smooth);
}

double esi_feed(struct esi *e, double value)
{
  double ret = mobile_feed(&e->smooth, value - e->last);
  e->last = value;

  return ret;
}

double esi_compute(struct esi *e)
{
  return mobile_average(&e->smooth);
}
