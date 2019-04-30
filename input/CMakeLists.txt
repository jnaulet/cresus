cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus input
add_library(input OBJECT
  yahoo_v7.c
  b4b.c
  mdgms.c
  xtrade.c
  euronext.c
  kraken.c
  )
