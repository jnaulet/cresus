cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus input
add_library(quotes OBJECT
  quotes.c
  b4b.c
  euronext.c
  yahoo_v7.c
  mdgms.c
  xtrade.c
  kraken.c
  )
