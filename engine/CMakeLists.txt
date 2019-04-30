cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus engine
add_library(engine OBJECT
  common_opt.c
  engine_v2.c
#  order.c
#  position.c
  )
