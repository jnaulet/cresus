cmake_minimum_required(VERSION 2.8)
project(cresus)

# flags
set(CMAKE_C_FLAGS "-fPIC -std=gnu99")

# includes 
include_directories(.)

# subdirs for cresus project
add_subdirectory(framework)
add_subdirectory(engine)
add_subdirectory(math)
add_subdirectory(indicator)
add_subdirectory(input)
add_subdirectory(sim)

add_library(cresus SHARED
  $<TARGET_OBJECTS:framework>
  $<TARGET_OBJECTS:engine>
  $<TARGET_OBJECTS:math>
  $<TARGET_OBJECTS:indicator>
  $<TARGET_OBJECTS:input>
  )

add_subdirectory(sample)
