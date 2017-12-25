cmake_minimum_required(VERSION 2.8)
project(cresus)

# flags
set(CMAKE_BUILD_TYPE Debug)

# src for cresus systems
add_executable(buy_red_filtered buy_red_filtered.c)
target_link_libraries(buy_red_filtered cresus)

add_executable(buy_monthly buy_monthly.c)
target_link_libraries(buy_monthly cresus)

# make install
install(TARGETS buy_red_filtered DESTINATION bin)
install(TARGETS buy_monthly DESTINATION bin)
