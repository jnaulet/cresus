cmake_minimum_required(VERSION 2.8)
project(cresus)

# Src for cresus framework
add_library(framework OBJECT
  indicator.c
  verbose.c
  time64.c
  track.c
  timeline_v2.c
  )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/framework
        DESTINATION include/cresus
        FILES_MATCHING PATTERN "*.h")
