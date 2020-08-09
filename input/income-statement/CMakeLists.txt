cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus input
add_library(income_statement OBJECT
  income-statement.c
  eodhistoricaldata.c
  )
