cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus input
add_library(cash_flow OBJECT
  cash-flow.c
  eodhistoricaldata.c
  )
