cmake_minimum_required(VERSION 2.8)
project(cresus)

# Src for cresus indicators
add_library(indicator OBJECT
  atr.c
  bollinger.c
  heikin_ashi.c
  jtrend.c
  macd.c
  mobile.c
  roc.c
  rs_dorsey.c
  rsi.c
  rs_mansfield.c
  smi.c
  srsi.c
  stochastic.c
  stoploss.c
  swing.c
  updown.c
  zigzag.c
  )

