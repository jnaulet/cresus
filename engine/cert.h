/*
 * Cresus EVO - cert.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 09.05.2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef CERT_H
#define CERT_H

/* Certificate/Turbo */
struct cert {
  double funding;
  double stoploss;
  double ratio;
};

static inline void cert_zero(struct cert *cert)
{
  cert->funding = 0.0;
  cert->stoploss = 0.0;
  cert->ratio = 1.0;
}

#define cert_long_value(cert, value)			\
  (((value) - ((cert)->funding)) / (cert)->ratio)
#define cert_short_value(cert, value)			\
  ((((cert)->funding) - (value)) / (cert)->ratio)

#endif
