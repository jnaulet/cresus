//
//  event.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 21/10/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#ifndef EVENT_H
#define EVENT_H

/* Event list */

typedef enum {
  
  /* Stoploss */
  EVENT_STOPLOSS_HIT,
  
  /* Mobile average */
  EVENT_MOBILE_CHDIR_UP,
  EVENT_MOBILE_CHDIR_DOWN,
  EVENT_MOBILE_CROSSED_UP,
  EVENT_MOBILE_CROSSED_DOWN,
  
  /* Zigzag */
  EVENT_ZIGZAG_CHDIR,
  
  /* Bollinger */
  EVENT_BOLLINGER_BOTTOM_HIT,
  EVENT_BOLLINGER_TOP_HIT,
  
  /* Heikin-Ashi */
  EVENT_HEIKIN_ASHI_CHDIR,
  
  /* Anything you want */
  
} event_t;

#endif
