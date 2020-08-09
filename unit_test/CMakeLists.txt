cmake_minimum_required(VERSION 2.8)
project(cresus)

# flags
set(CMAKE_BUILD_TYPE Debug)

# src for cresus systems
add_executable(track_add_balance_sheet track_add_balance_sheet.c)
target_link_libraries(track_add_balance_sheet cresus)
add_executable(track_add_income_statement track_add_income_statement.c)
target_link_libraries(track_add_income_statement cresus)
add_executable(track_add_cash_flow track_add_cash_flow.c)
target_link_libraries(track_add_cash_flow cresus)
