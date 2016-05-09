cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus engine
add_library(engine OBJECT
  calendar.c
  candle.c
  engine.c
  order.c
  position.c
  timeline.c
  )
