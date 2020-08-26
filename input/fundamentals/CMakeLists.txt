cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus input
add_library(fundamentals OBJECT
  fundamentals.c
  eodhistoricaldata.c
  )
