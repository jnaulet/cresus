cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus input
add_library(splits OBJECT
  splits.c
  eodhistoricaldata.c
  )
