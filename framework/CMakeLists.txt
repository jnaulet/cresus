cmake_minimum_required(VERSION 2.8)
project(cresus)

# Src for cresus framework
add_library(framework OBJECT
  indicator.c
  timeline_entry.c
  verbose.c
  )
