cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus input
add_library(math OBJECT
  average.c
  )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/math
        DESTINATION include/cresus
        FILES_MATCHING PATTERN "*.h")
