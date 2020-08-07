cmake_minimum_required(VERSION 2.8)
project(cresus)

# src for cresus engine
add_library(engine OBJECT
  engine_v2.c
  portfolio.c
  )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/engine
        DESTINATION include/cresus
        FILES_MATCHING PATTERN "*.h")
